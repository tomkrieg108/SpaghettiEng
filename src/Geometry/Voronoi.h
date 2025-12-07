#pragma once
#include "Geometry/RBTree.h"
#include "Geometry/RBTreeTraversable.h"
#include "Geometry/DCEL.h"
#include "Geometry/GeomUtils.h"
#include <spdlog/spdlog.h> // format string for Voronoi Node
#include <queue>
#include <variant>
#include <tuple> //for std::tie


#define VORONOI_VERS 1

namespace Geom
{
  //Todo: Template for float or double?
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

#if (VORONOI_VERS==1)

  class Voronoi
  {
    
  private:

    template<typename,typename,typename> friend struct ::fmt::formatter;
    struct Node;
    struct SiteEvent;
    struct CircleEvent;
    
    struct NodeCompare
    {
      NodeCompare(Voronoi const* const ctx ) : ctx{ctx} {}
      bool operator ()(Node const & n1,  Node const& n2) const noexcept;
      private:
        bool CompArcToArc(Node const & arc1,  Node const& arc2) const noexcept; 
        bool CompArcToBP(Node const & arc,  Node const& bp) const noexcept; 
        bool CompBPToBP(Node const & bp1,  Node const& bp2) const noexcept; 
      private:  
        Voronoi const* const ctx;
    };

    using Beachline =  Geom::RBTreeTraversable<Node,void,NodeCompare>;
    using BeachItr = Beachline::Iterator;
    using NodeHandle = Beachline::NodeHandle;

    struct Node
    {
      enum class Type {Internal, Leaf};

      Type type = Type::Leaf;
      uint32_t unique_id;

      //For Internal nodes (breakpoints)
      Point2d site_left{0,0};
      Point2d site_right{0,0};
      NodeHandle arc_left{};
      NodeHandle arc_right{};
      DCEL::HalfEdge* half_edge = nullptr; 

      //For Leaf nodes (arcs)
      Point2d site{0,0};
      NodeHandle break_left{};
      NodeHandle break_right{};
      CircleEvent* circle_event = nullptr;

      static std::string ToString(Node const & n);
      Voronoi* ctx = nullptr; //To get access to y_sweep, used in ToString() only
    }; 

    struct SiteEvent
    {
      SiteEvent() = default;
      SiteEvent(float x, float y): site{x,y} {}
      SiteEvent(Point2d const& p): site{p} {}
      Point2d site;
      Point2d GetPoint() const {return site;}
    };

    struct CircleEvent
    {
      CircleEvent() = default;
      CircleEvent(const Point2d& p) : circle_bottom{p} {}
      Point2d circle_bottom;
      NodeHandle leaf_going{};
      bool false_alarm = false;
      Point2d GetPoint() const {return circle_bottom;}
    };

    using Event = std::variant<SiteEvent, CircleEvent>;

    struct EventCompare {
      bool operator()(const Event& a, const Event& b) const noexcept;
    };

   
  public:
    Voronoi() = default;
    Voronoi(std::vector<Geom::Point2d>& points);
    static void Test();

  private:
 
    void Build(std::vector<Geom::Point2d>& points);
    void HandleSiteEvent(SiteEvent e);
    void HandleCircleEvent(CircleEvent e);
    static float ComputeBreakpointX(const Node& node, float y_sweep);
    void GetArcTriples(const Node& arc);
    uint32_t NextId() {return node_id++;}

    // For testing / validation
    void AddEventsToQueue(){}
    void PrintQueue(){}
    void PrintBeach();

  private:
    Beachline m_beach = Beachline{NodeCompare{this}};
    std::priority_queue<Event,std::vector<Event>,EventCompare> m_event_queue;
    Geom::DCEL m_diagram;
    float m_sweep{0};
    uint32_t node_id{0};
  };

  #endif


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

#if 0
    struct BeachNodeComp
    {
      BeachNodeComp(Voronoi* ctx ) : ctx{ctx} {}
      bool operator ()(BeachNode* node1,  BeachNode* node2) const noexcept;
      private:
        bool CompArcToBP(BeachNode* arc_node,  BeachNode* bp_node) const noexcept; 
        bool CompBPToBP(BeachNode* bp1_node,  BeachNode* bp2_node) const noexcept; 
      private:  
        Voronoi* ctx;
    };
#endif

    struct Arc
    {
      Point2d site{0,0};
      CircleEvent* circle_event = nullptr;
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
      
      BeachElement() : data{Arc()} {}
      BeachElement(Arc arc) : data{arc} {}
      BeachElement(Breakpoint bp) : data{bp} {}

      bool IsArc() const {
        return std::holds_alternative<Arc>(data);
      }
      bool IsBreakpoint() const {
        return std::holds_alternative<Breakpoint>(data);
      }
      Arc GetArc() const {
        SPG_ASSERT(IsArc());
        return std::get<Arc>(data);
      }
      Breakpoint GetBreakpoint() const {
        SPG_ASSERT(IsBreakpoint());
        return std::get<Breakpoint>(data);
      }

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

      std::string ToString(Voronoi* ctx);

      private:
        static uint32_t next_id;
    };

    struct BeachNode : public RBNodeBase<BeachNode,BeachElement>
    {
      using Base = RBNodeBase<BeachNode,BeachElement>;
      using Base::Base;
      BeachElement& GetElement() {return value;}
      bool IsArc() { return value.IsArc();}
      bool IsBreakpoint() { return value.IsBreakpoint();}
      Arc GetArc() {return value.GetArc();}
      Breakpoint GetBreakpoint() {return value.GetBreakpoint();}
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

      //Note:  Next two can't be static because they need member of 'this', m_nil
      BeachNode* MakeArcNode(Point2d site) {
        BeachElement arc_el = BeachElement::MakeArcElement(site);
        return Base::MakeNode(arc_el,m_nil);
      }

      BeachNode* MakeBreakpointNode(Point2d site_left,Point2d site_right) {
        BeachElement bp_el = BeachElement::MakeBreakpointElement(site_left,site_right);
        return Base::MakeNode(bp_el,m_nil);
      }
    };

    struct SiteEvent
    {
      SiteEvent() = default;
      SiteEvent(float x, float y): site{x,y} {}
      SiteEvent(Point2d const& p): site{p} {}
      Point2d GetPoint() const {return site;}
      Point2d site;
    };

    struct CircleEvent
    {
      CircleEvent() = default;
      CircleEvent(float x, float y): circle_low{x,y} {}
      CircleEvent(const Point2d& p) : circle_low{p} {}
      Point2d GetPoint() const {return circle_low;}
      Point2d circle_low;
      BeachNode* arc_node_going = nullptr;
      bool false_alarm = false;
    };

    using Event = std::variant<SiteEvent, CircleEvent>;
    using EventPtr = std::unique_ptr<Event>;

    struct EventCompare {
      bool operator()(const EventPtr& a, const EventPtr& b) const noexcept;
    };

    public:
      Voronoi() = default;
      Voronoi(std::vector<Geom::Point2d>& points);

      void Build(std::vector<Geom::Point2d>& points);
      void HandleSiteEvent(SiteEvent e);
      void HandleCircleEvent(CircleEvent e);

      float ComputeBreakpointX(const Breakpoint& bp);

      //void GetArcTriples(const Node& arc);
     

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

} //namespace Geom

// template<>
// struct fmt::formatter<Geom::Voronoi_V2::Voronoi::BeachElement> {
//   constexpr auto parse(format_parse_context& ctx) {
//     return ctx.begin();
//   }

//   template <typename FormatContext>
//   auto format(const Geom::Voronoi_V2::Voronoi::BeachElement& e, FormatContext& ctx) const  {
//     return fmt::format_to(ctx.out(), "{}", Geom::Voronoi_V2::Voronoi::BeachElement::ToString(e));
//   }
// };


#if (VORONOI_VERS==1)
//Need this so that can do SPG_TRACE(node) etc.  Needs to be in global NS
template<>
struct fmt::formatter<Geom::Voronoi::Node> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Geom::Voronoi::Node& n, FormatContext& ctx) const  {
    return fmt::format_to(ctx.out(), "{}", Geom::Voronoi::Node::ToString(n));
  }
};
#endif

#if (VORONOI_VERS==2)
template<>
struct fmt::formatter<Geom::Voronoi::Node> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Geom::Voronoi::Node& n, FormatContext& ctx) const  {
    return fmt::format_to(ctx.out(), "{}", std::string{"A Beach Node"});
  }
};
#endif


//NOTE: std::less<>
/*
  std:less (roughly):

  template<class T = void>
  struct less;

  specializtion for T = void:

  template<>
  struct less<void> {

      Tells STL that the comparator can compare different but comparable types
      using is_transparent = void; 

      template<class A, class B>
      constexpr bool operator()(A&& a, B&& b) const
          noexcept(noexcept(std::forward<A>(a) < std::forward<B>(b)))
      {
          return std::forward<A>(a) < std::forward<B>(b);
      }
  };

  Notice how the operator is a template — it can deduce A and B automatically.

  It means you can use std::less<> (or std::greater<>, etc.) without ever mentioning your custom type explicitly.
*/