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
      return (a*x*x + b*x + c);
    }
    bool IsDegenerate() const noexcept {return is_degenerate;}

    private:
      double focx,focy,dirx;
      bool is_degenerate;
  };

  Point2d ComputeBreakpoint(Point2d const& left_site, Point2d const& right_site, float sweep_y);

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
    //Following only used if type == circle
    Arc* diappearing_arc = nullptr; 
    CircleData circle;
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
      bool operator()(Event* e1,  Event* e2) const {
        if(e1->point->y < e2->point->y)
          return true;
        if(e2->point->y < e1->point->y)
          return false;
        return e1->point->x < e2->point->x;
      }
    };
  private:
    std::priority_queue<Event*,std::vector<Event*>, EventCompare> m_queue;  
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

  //Not used
  struct BreakpointPair
  {
    Breakpoint* left = nullptr;
    Breakpoint* right = nullptr;
  };

  class BeachTree: public RBTree_V2::RBTree<BeachElement,BeachElementComp>
  {
    
  public:  
    using Base = RBTree<BeachElement,BeachElementComp>;
    using BeachNode = typename Base::node_type;
    using Base::Base;

    using NodeList = std::array<BeachNode*,5>; // arc,bp,arc,bp,arc
    using ArcTriple = std::array<Arc*,3>;
    
  private:
    friend class Voronoi;

    Voronoi* ctx = nullptr; //Passed to BeachElement structs in Makexxx()
    
    void SetComparator(BeachElementComp comp) {this->m_comp = comp;} //Not used
    BeachNode* MakeArcNode(Point2d const * site);
    BeachNode* MakeBreakpointNode(Arc* left_arc, Arc* right_arc); //Not used
    BeachNode* MakeBreakpointNode();

    BeachNode* FindArcNodeAbove(Point2d const * site, float sweep_y);
    NodeList MakeNodeList(Event* site_event, BeachNode* arc_node_above);
    void InsertNodeList(Event* site_event, NodeList& 
      node_list, BeachNode* arc_node_above);

    BreakpointPair GetLeftBreakpointPair(Arc* arc); //Not used
    BreakpointPair GetRightBreakpointPair(Arc* arc); //Not used
    BreakpointPair GetBreakpointPair(Arc* arc); //Not used

    ArcTriple GetArcTriple(Arc* middle_Arc);
   

    //Todo: Make these static as appropriate!
    //Helpers
    bool IsArc(BeachNode* node);
    bool IsBreakpoint(BeachNode* node);
    Arc* GetArc(BeachNode* arc_node);
    Breakpoint* GetBreakpoint(BeachNode* bp_node);

    void SetCircleEventValidity(BeachNode* arc_node, bool value); //Not used  

    void SetArcNeighbours(BeachNode* arc_node, 
      Breakpoint* bp_left, Breakpoint* bp_right);
    void SetBreakpointNeighbours(BeachNode* bp_node, 
      Arc* arc_left, Arc* arc_right);

    void SetArcNeighbours(BeachNode* arc_node, 
      BeachNode* bp_node_left, BeachNode* bp_node_right); //Not used  
    void SetBreakpointNeighbours(BeachNode* bp_node, 
      BeachNode* arc_node_left, BeachNode* arc_node_right); //Not used     

    Arc* LeftArc(BeachNode* node);
    Arc* RightArc(BeachNode* node);
    Breakpoint* LeftBreakpoint(BeachNode* node);
    Breakpoint* RightBreakpoint(BeachNode* node);
  };

  struct Arc 
  {
    Point2d const * site = nullptr;
    Event* circle_event = nullptr;
    //links to neighbouring breakpoints:
    Breakpoint* left_bp = nullptr;
    Breakpoint* right_bp = nullptr;
    //Link to associated tree node
    BeachTree::BeachNode* tree_node = nullptr;

    //For Validation / debugging:
    uint32_t id;
    static std::string ToString(Arc* arc,float sweep_y);
  };

  struct Breakpoint
  {
    Arc* left_arc = nullptr;
    Arc* right_arc = nullptr;
    DCEL::HalfEdge* half_edge = nullptr;
    //Link to associated tree node
    BeachTree::BeachNode* tree_node = nullptr;

    float CurrentX(float sweep_y) const {
      return ComputeBreakpoint(*left_arc->site, *right_arc->site, sweep_y).x;
    }
    Point2d CurrentPos(float sweep_y) const {
      return ComputeBreakpoint(*left_arc->site, *right_arc->site, sweep_y);
    }
    //For Validation / debugging:
    uint32_t id;
    static std::string ToString(Breakpoint* bp,float sweep_y);
  };

  class Voronoi
  {
    friend class BeachTree;
  public:
    Voronoi() = default;
    Voronoi(std::vector<Point2d> points);
    void Construct();
    float GetSweepY() {return m_sweep;}
    BeachTree& GetBeachTree() {return m_beach;}
  
    std::vector<Point2d> GetConnectedEdgePoints();
    std::vector<Point2d> GetLooseEdgePoints();
    std::vector<Point2d> GetVertexPoints();

    // For testing / validation
    void PrintBeach();
    static void Test();

  private:
    
    void HandleSiteEvent(Event* e);
    void HandleCircleEvent(Event* e);
    void TieLooseEnds();
    void TryInsertCircleEvent(BeachTree::ArcTriple const& arc_triple);
    void InsertCircleEventIfNeeded(BreakpointPair bp_pair, Arc* new_arc); //Not used
    
    Arc* MakeArc(Point2d const* site_point);
    Breakpoint* MakeBreakpoint();
    Event* MakeCircleEvent(Point2d const& point, CircleData const& circle,
       Arc* disappearing_arc);

  private:
    //Filled on initialization
    std::vector<Point2d> m_points; 

    // Added to during runtime
    std::vector<std::unique_ptr<Arc>> m_arcs;
    std::vector<std::unique_ptr<Breakpoint>> m_breakpoints;
    std::vector<std::unique_ptr<Event>> m_circle_events;
    std::vector<std::unique_ptr<Point2d>> m_circle_event_points;

    // bounding box containing all vertices of Voronoi diagram
    Geom::BoundingBox m_bounding_box;
    
    EventQueue m_event_queue; //Initialized from m_points in constructor
    BeachTree m_beach = BeachTree(BeachElementComp(this));
    Geom::DCEL m_dcel;
    float m_sweep = 0;
    float m_sweep_prev = 0;
  };

} //namespace Voronoi_V3


} //namespace Geom

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