#pragma once
#include "Geometry/RBTree.h"
#include "Geometry/RBTreeTraversable.h"
#include "Geometry/DCEL.h"
#include "Geometry/GeomUtils.h"
#include <spdlog/spdlog.h> // format string for Voronoi Node
#include <array>
#include <queue>
#include <variant>
#include <tuple> //for std::tie




namespace Geom
{
  
  struct Parabola
  {
    double a,b,c; //i.e. y=ax^2 + bx + c

    Parabola() = delete;
    Parabola(Point2d focus, float directrix) :
      focx{static_cast<double>(focus.x)},
      focy{static_cast<double>(focus.y)},
      dirx{static_cast<double>(directrix)}
     {
      is_degenerate = (Equal(focy, dirx));
      if(is_degenerate) 
        a=b=c=0;
      else {
        a = 0.5/(focy - dirx);
        b = -2*focx*a;
        c = (focx*focx + focy*focy - dirx*dirx)*a;
      }
     }

    float GetY(double x) const noexcept {
      return static_cast<float>(a*x*x + b*x + c);
    }
    double GetYd(double x) const noexcept {
      double xd = static_cast<double>(x);
      return (a*xd*xd + b*xd + c);
    }
    bool IsDegenerate() const noexcept {return is_degenerate;}

    private:
      double focx,focy,dirx;
      bool is_degenerate;
  };

  float ComputeBreakpointX(Point2d const& left_site, Point2d const& right_site, float sweep_y);

namespace Voronoi_V2
{
  class Voronoi 
  {
    template<typename,typename,typename> friend struct ::fmt::formatter;

    struct Arc;
    struct Breakpoint;
    struct BeachElement;  // Payload 
    struct BeachNode; // Tree Node itself (payload plus pointers for navigation)
    struct BeachTree;
    struct SiteEvent;
    struct CircleEvent;

    template<typename TNode, typename TValue>
    using RBNodeBase = Geom::RBTree_V2::RBNodeBase<TNode,TValue>;

    template<
      typename TValue, 
      typename TComp = std::less<TValue>,
      typename TNode = void
    >
    using RBTree = Geom::RBTree_V2::RBTree<TValue,TComp,TNode>;
    
    
    struct BeachElementComp
    {
      BeachElementComp() = default; //TODO:  Without this is doesn't compile - investigate
      BeachElementComp(Voronoi*  ctx ) : ctx{ctx} {}
      bool operator ()(BeachElement const & n1,  BeachElement const& n2) const noexcept;
      private:
        bool CompArcToBP(BeachElement const & arc,  BeachElement const& bp) const noexcept; 
        bool CompBPToBP(BeachElement const & bp1,  BeachElement const& bp2) const noexcept; 
      private:  
        Voronoi* ctx;
    };

    struct Arc
    {
      Point2d site{0,0};
      CircleEvent* circle_event = nullptr; // The circle event at which this arc disappears
    };
 
    struct Breakpoint
    {
      Point2d site_left{0,0};
      Point2d site_right{0,0};
      DCEL::HalfEdge* half_edge = nullptr;
    };

    struct BeachElement
    {
      std::variant<Arc,Breakpoint> data;
      uint32_t element_id = 0;
      BeachNode* prev_node = nullptr; 
      BeachNode* next_node = nullptr;
      static uint32_t next_id;
      Voronoi* ctx = nullptr; //For access to y_sweep, used in ToString() only
      
      BeachElement() : data{Arc()} {}
      BeachElement(Arc arc) : data{arc} {}
      BeachElement(Breakpoint bp) : data{bp} {}

      //Todo: use std::get_if()? - only 1 check.  returns a pointer insteaf of ref
      bool IsArc() const {return std::holds_alternative<Arc>(data);}
      bool IsBreakpoint() const {return std::holds_alternative<Breakpoint>(data);}
      Arc const& GetArc() const {return std::get<Arc>(data);}
      Arc& GetArc() {return std::get<Arc>(data);}
      Breakpoint const& GetBreakpoint() const {return std::get<Breakpoint>(data);}
      Breakpoint& GetBreakpoint() {return std::get<Breakpoint>(data);}

      static BeachElement MakeArcElement(Point2d site) {
        Arc arc = Arc{site,nullptr};
        BeachElement element;
        element.element_id = next_id++;
        element.data = arc;
        return element;
      }
      static BeachElement MakeBreakpointElement(Point2d left, Point2d right) {
        Breakpoint bp = Breakpoint{left,right,nullptr};
        BeachElement element;
        element.element_id = next_id++;
        element.data = bp;
        return element;
      }
      static std::string ToString(BeachElement const & n);
    };

    struct BeachNode : public RBNodeBase<BeachNode,BeachElement>
    {
      using Base = RBNodeBase<BeachNode,BeachElement>;
      using Base::Base;

      bool IsArc() const { return value.IsArc();}
      bool IsBreakpoint() const { return value.IsBreakpoint();}
      BeachElement const& GetElement() const {return value;}
      BeachElement& GetElement() {return value;}
      Arc const& GetArc() const {return value.GetArc();}
      Arc& GetArc() {return value.GetArc();}
      Breakpoint const& GetBreakpoint() const {return value.GetBreakpoint();}
      Breakpoint& GetBreakpoint() {return value.GetBreakpoint();}
      
      BeachNode* GetPrevNode() {
        SPG_ASSERT(value.prev_node != nullptr);
        return value.prev_node;
      }
      BeachNode* GetNextNode() {
        SPG_ASSERT(value.next_node != nullptr);
        return value.next_node;
      }
      void SetNextNode(BeachNode* next) {
        SPG_ASSERT(next != nullptr);
        value.next_node = next;
      }
      void SetPrevNode(BeachNode* prev) {
        SPG_ASSERT(prev != nullptr);
        value.prev_node = prev;
      }
      void SetCircleEvent(CircleEvent* e) {
        SPG_ASSERT(e != nullptr);
      }
      void SetPrevAndNextNodes(BeachNode* prev,BeachNode* next) {
        SetPrevNode(prev);
        SetNextNode(next);
      }
    };

    class BeachTree: public RBTree<BeachElement,BeachElementComp,BeachNode>
    {
      friend class Voronoi;
      using Base = RBTree<BeachElement,BeachElementComp,BeachNode>;
      using Base::Base;
      
      BeachNode* FindArcVerticallyAbovePoint(Point2d point, Voronoi* ctx);
      BeachNode* GetNilNode() {return m_nil;}

  #if 0
      Voronoi* ctx = nullptr;
      template<typename... Args>
      BeachTree(Voronoi* ctx, Args&&... args): 
        ctx{ctx},
        Base(std::forward<Args>(args)...) {}
  #endif

      //Note:  Next two can't be static because they need non-static member m_nil, 
      BeachNode* MakeArcNode(Point2d site, Voronoi* ctx) {
        BeachElement arc_el = BeachElement::MakeArcElement(site);
        arc_el.prev_node = arc_el.next_node = m_nil;
        arc_el.ctx = ctx;
        return Base::MakeNode(arc_el,m_nil);
      }

      BeachNode* MakeBreakpointNode(Point2d site_left,Point2d site_right, Voronoi* ctx) {
        BeachElement bp_el = BeachElement::MakeBreakpointElement(site_left,site_right);
        bp_el.prev_node = bp_el.next_node = m_nil;
        bp_el.ctx = ctx;
        return Base::MakeNode(bp_el,m_nil);
      }
    };

    struct SiteEvent
    {
      Point2d site;

      SiteEvent() = default;
      SiteEvent(float x, float y): site{x,y} {}
      SiteEvent(Point2d const& p): site{p} {}
      Point2d GetPoint() const {return site;}
    };

    struct CircleEvent
    {
      Point2d circle_point;
      BeachNode* arc_node_going = nullptr;
      bool false_alarm = false;

      CircleEvent() = default;
      CircleEvent(float x, float y): circle_point{x,y} {}
      CircleEvent(const Point2d& p) : circle_point{p} {}
      Point2d GetPoint() const {return circle_point;}
    };

    using Event = std::variant<SiteEvent, CircleEvent>;
    using EventPtr = std::unique_ptr<Event>;

    struct EventCompare {
      bool operator()(const EventPtr& a, const EventPtr& b) const noexcept;
    };

    struct NodePair
    {
      BeachNode* left = nullptr;
      BeachNode* right = nullptr;
    };

    public:
      Voronoi() = default;
      Voronoi(std::vector<Geom::Point2d>& points);
      void Build(std::vector<Geom::Point2d>& points);

    private:  
      void HandleSiteEvent(SiteEvent e);
      void HandleCircleEvent(CircleEvent e);
      float ComputeBreakpointX(const Breakpoint& bp);
      NodePair GetBreakpointPairleft(BeachNode* arc_node);
      NodePair GetBreakpointPairRight(BeachNode* arc_node);
      void AddCircleEventIfRequired(NodePair bp_pair, BeachNode* new_arc);

    public:
      // For testing / validation
      void PrintBeach();
      static void Test();
      
    private:
      BeachTree m_beach = BeachTree{BeachElementComp{this}};
      std::priority_queue<EventPtr,std::vector<EventPtr>,EventCompare> m_event_queue;
      Geom::DCEL m_dcel;
      float m_sweep{0};
  };
} //namespace Voronoi_V2

namespace Voronoi_V3 
{
  class Voronoi;
  struct Arc;
  struct Breakpoint;
  struct BeachElement;
  
  struct CircleData {
    Point2d center;
    float radius;
  };

  struct Event
  {
    enum class Type {Site, Circle};
    Type type;
    Point2d const* point = nullptr; //Used for both Site and Circle events
    //Following is used if type == circle
    Arc* diappearing_arc = nullptr; 
    CircleData circle; //optional - not strictly needed
    bool valid = true; // Circle events can get invalidated
  };

  class EventQueue 
  {
    friend class Voronoi;
  public:

    void Initialize(std::vector<Point2d> const& points);

    void Push(Event* e) {
      m_queue.push(e);
    }
    Event* Pop() {
      Event* top_event = m_queue.top();
      m_queue.pop();
      return top_event;
    }
    bool IsEmpty() const {
      return m_queue.empty();
    }

  private:
    struct EventCompare {
      // bool operator()(const Event& a, const Event& b) const {
      //   return a.y < b.y;
      // }
      bool operator()(Event* e1,  Event* e2) const {
        if(e1->point->y < e2->point->y)
          return true;
        if(e2->point->y < e1->point->y)
          return false;
        return e1->point->x < e2->point->x;
      }
    };
  private:
    //std::priority_queue<Event,std::vector<Event>, EventCompare> m_queue;  
    std::priority_queue<Event*,std::vector<Event*>, EventCompare> m_queue;  
  };

  struct Arc 
  {
    Point2d const * site = nullptr;
    Event* circle_event = nullptr;
    //links to neighbouring breakpoints:
    Breakpoint* left_bp = nullptr;
    Breakpoint* right_bp = nullptr;

    //For Validation / debugging:
    BeachElement* element = nullptr;
    uint32_t id;
  };

  struct Breakpoint
  {
    Arc* left_arc = nullptr;
    Arc* right_arc = nullptr;
    DCEL::HalfEdge* half_edge = nullptr;

    float CurrentX(float sweep_y) const {
      return ComputeBreakpointX(*left_arc->site, *right_arc->site, sweep_y);
    }

    //For Validation / debugging:
    BeachElement* element = nullptr;
    uint32_t id;
  };

  struct BeachElement
  {
    template<typename,typename,typename> friend struct ::fmt::formatter;

    bool is_arc = true;
    Arc* arc = nullptr;
    Breakpoint* breakpoint = nullptr;
    static uint32_t next_id;

    //For logger (need ctx->m_sweep to calculate and display cur x-pos)
    Voronoi* ctx = nullptr; 
    static std::string ToString(BeachElement const & el);
  };

  struct BeachElementComp
  { 
    Voronoi* ctx = nullptr;  //need ctx->m_sweep to calculate cur x-pos

    BeachElementComp() = default;
    BeachElementComp(Voronoi* ctx) : ctx{ctx} {}
    void SetContext(Voronoi* ctx) {this->ctx = ctx;} //Todo: Not used
    bool operator () (BeachElement const& el1, BeachElement const& el2) const;
    bool CompArcToArc(BeachElement const& arc1,  BeachElement const& arc2) const;
    bool CompArcToBP(BeachElement const& arc,  BeachElement const& bp) const; 
    bool CompBPToBP(BeachElement const& bp1,  BeachElement const& bp2) const;
  };

  struct BreakpointPair
  {
    Breakpoint* left = nullptr;
    Breakpoint* right = nullptr;
  };

  class BeachTree: public RBTree_V2::RBTree<BeachElement,BeachElementComp>
  {
    friend class Voronoi;

    Voronoi* ctx = nullptr; //Passed to BeachElement structs in Makexxx()
    using Base = RBTree<BeachElement,BeachElementComp>;
    using BeachNode = typename Base::node_type;
    using Base::Base;

    void SetComparator(BeachElementComp comp) {this->m_comp = comp;} //Not used
    BeachNode* MakeArcNode(Point2d const * site);
    BeachNode* MakeBreakpointNode(Arc* left_arc, Arc* right_arc); //Not used
    BeachNode* MakeBreakpointNode();

    using ArcTriple = std::array<BeachNode*,5>;

    BeachNode* FindArcNodeAbove(Point2d const * site, float sweep_y);
    ArcTriple MakeArcTriple(Event* site_event, BeachNode* replaced_arc_node, float sweep_y);
    void InsertArcTriple(Event* site_event, ArcTriple& arc_triple, BeachNode* replaced_arc_node);

    BreakpointPair GetLeftBreakpointPair(Arc* arc);
    BreakpointPair GetRightBreakpointPair(Arc* arc);
   
    //Not yet implemented
    void RemoveDissapearingArc(BeachNode* arc_node) {}
    void RemoveArc(BeachNode* arc_node){}

    //Todo: Make these static as appropriate!
    //Helpers
    bool IsArc(BeachNode* node);
    bool IsBreakpoint(BeachNode* node);
    Arc* GetArc(BeachNode* arc_node);
    Breakpoint* GetBreakpoint(BeachNode* bp_node);

    void SetCircleEventValidity(BeachNode* arc_node, bool value);  

    void SetArcNeighbours(BeachNode* arc_node, 
      Breakpoint* bp_left, Breakpoint* bp_right);
    void SetBreakpointNeighbours(BeachNode* bp_node, 
      Arc* arc_left, Arc* arc_right);

     //These not used (yet)  
    void SetArcNeighbours(BeachNode* arc_node, 
      BeachNode* bp_node_left, BeachNode* bp_node_right);
    void SetBreakpointNeighbours(BeachNode* bp_node, 
      BeachNode* arc_node_left, BeachNode* arc_node_right);  

    Arc* LeftArc(BeachNode* node);
    Arc* RightArc(BeachNode* node);
    Breakpoint* LeftBreakpoint(BeachNode* node);
    Breakpoint* RightBreakpoint(BeachNode* node);
    //Arc* NthLeftArc(BeachNode* node, uint32_t n);
    //Arc* NthRightArc(BeachNode* node, uint32_t n);
    //Breakpoint* NthLeftBreakpoint(BeachNode* node, uint32_t n);
    //Breakpoint* NthRightBreakpoint(BeachNode* node, uint32_t n);
  };

  class Voronoi
  {
  public:
    Voronoi() = default;
    Voronoi(std::vector<Point2d> points);
    float GetSweepY() {return m_sweep;}
    BeachTree& GetBeachTree() {return m_beach;}

    // For testing / validation
    void PrintBeach();
    static void Test();

  private:
    void Construct();  
    void HandleSiteEvent(Event* e);
    void HandleCircleEvent(Event* e);
    void InsertCircleEventIfNeeded(BreakpointPair bp_pair, Arc* new_arc);

  private:
    std::vector<Point2d> m_points;
    EventQueue m_event_queue;
    BeachTree m_beach = BeachTree(BeachElementComp(this));
    Geom::DCEL m_dcel;
    float m_sweep = 0;
  };

} //namespace Voronoi_V3


} //namespace Geom

template<>
struct fmt::formatter<Geom::Voronoi_V2::Voronoi::BeachElement> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Geom::Voronoi_V2::Voronoi::BeachElement& e, FormatContext& ctx) const  {
    return fmt::format_to(ctx.out(), "{}", Geom::Voronoi_V2::Voronoi::BeachElement::ToString(e));
  }
};

template<>
struct fmt::formatter<Geom::Voronoi_V3::BeachElement> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Geom::Voronoi_V3::BeachElement& e, FormatContext& ctx) const  {
    return fmt::format_to(ctx.out(), "{}", Geom::Voronoi_V3::BeachElement::ToString(e));
  }
};