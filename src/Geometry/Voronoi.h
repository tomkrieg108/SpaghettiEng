#pragma once
#include "Geometry/RBTree.h"
#include "Geometry/RBTreeTraversable.h"
#include "Geometry/DCEL.h"
#include "Geometry/GeomUtils.h"
#include <spdlog/spdlog.h> // format string for Voronoi Node
#include <array>
#include <queue>
#include <tuple> //for std::tie


namespace Geom
{
  
  struct Parabola
  {
    double a,b,c; //i.e. y=ax^2 + bx + c, or x=c if degenerate

    Parabola() = delete;
    Parabola(Point2d focus, float directrix) {
      is_degenerate = (Equal(focus.y, directrix)); //equality of floats rather than doubles
      if(is_degenerate) {  // vertical line x = focx
        a=b=0;
        c = focx;
        return;
      }
      focx = static_cast<double>(focus.x);
      focy = static_cast<double>(focus.y);
      dirx = static_cast<double>(directrix);
      a = 0.5/(focy - dirx);
      b = -2*focx*a;
      c = (focx*focx + focy*focy - dirx*dirx)*a;
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

  namespace Voronoi_V4
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
      //Following only used for Circle events
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
      using XRankType = uint32_t;
      bool is_arc = true;
      Arc* arc = nullptr;
      Breakpoint* breakpoint = nullptr;
      //Tndicates relative x position on the beachline - used in comparator.  Each element has a unique value.
      uint32_t x_pos_rank = 0;
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
      bool operator () (BeachElement const& el1, BeachElement const& el2) const;
    };

    class BeachTree: public RBTree_V2::RBTree<BeachElement,BeachElementComp>
    {
      friend class Voronoi; 
      using XRankType = BeachElement::XRankType;
    public:  
      using Base = RBTree<BeachElement,BeachElementComp>;
      using BeachNode = typename Base::node_type;
      using Base::Base;
      using NodeList = std::array<BeachNode*,5>; // arc,bp,arc,bp,arc
      using ArcTriple = std::array<Arc*,3>;
      
    private:
      BeachNode* MakeArcNode(Point2d const * site);
      BeachNode* MakeBreakpointNode();
      BeachNode* FindArcNodeAbove(Point2d const * site, float sweep_y);
      NodeList MakeNodeList(Event* site_event, BeachNode* arc_node_above);
      void InsertNodeList(Event* site_event, NodeList& node_list, BeachNode* arc_node_above);
      ArcTriple GetArcTriple(Arc* middle_Arc);
      bool IsArc(BeachNode* node);
      bool IsBreakpoint(BeachNode* node);
      Arc* GetArc(BeachNode* arc_node);
      Breakpoint* GetBreakpoint(BeachNode* bp_node);
      void SetArcNeighbours(BeachNode* arc_node, Breakpoint* bp_left, Breakpoint* bp_right);
      void SetBreakpointNeighbours(BeachNode* bp_node,Arc* arc_left, Arc* arc_right);
      Arc* LeftArc(BeachNode* node);
      Arc* RightArc(BeachNode* node);
      Breakpoint* LeftBreakpoint(BeachNode* node);
      Breakpoint* RightBreakpoint(BeachNode* node);
      XRankType ResetPoitionalRankings();
    private:
      Voronoi* ctx = nullptr; //Passed to BeachElement structs in Makexxx()
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
      float CurrentX(float sweep_y);
      Point2d CurrentPos(float sweep_y);
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
      void TieLooseEnds(); //Currently not used
      void TryInsertCircleEvent(BeachTree::ArcTriple const& arc_triple);
    
      Arc* MakeArc(Point2d const* site_point);
      Breakpoint* MakeBreakpoint();
      Event* MakeCircleEvent(Point2d const& point, CircleData const& circle,
        Arc* disappearing_arc);
     
    public:
      Point2d ComputeBreakpointCoords(Breakpoint* bp);

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
      float m_sweep_prev = 0; //Not used
    };

} //namespace Voronoi_V4


} //namespace Geom

template<>
struct fmt::formatter<Geom::Voronoi_V4::BeachElement> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Geom::Voronoi_V4::BeachElement& e, FormatContext& ctx) const  {
    return fmt::format_to(ctx.out(), "{}", Geom::Voronoi_V4::BeachElement::ToString(e));
  }
};