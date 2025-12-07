#include "Voronoi.h"
#include <iostream>
#include <optional>
#include <format>

namespace Geom
{

  static std::optional<std::pair<double,double>> 
  ComputePolynomialZeros(double a, double b, double c) noexcept
  {
    // roots of ax^2 + bx + c
    if(Equal(a, 0)) {
      SPG_ASSERT(!Equal(static_cast<float>(b),0.0f));
      auto x = -c/b;
      return std::optional(std::pair(x,x));
    }
    auto discr = b*b - 4*a*c;
    if(discr < 0)
      return std::nullopt;
    auto discr_sqrt = std::sqrt(discr);
    auto x_left = (-b - discr_sqrt)/(2*a);
    auto x_right = (-b + discr_sqrt)/(2*a);
    return std::optional(std::pair(x_left,x_right));
  }

  //return intersection points of 2 parabolas if they exist
  static std::optional<std::pair<Point2d,Point2d>> 
  ComputeIntersections(const Parabola& p1, const Parabola& p2) noexcept 
  {
    //Note: if p1,p2 have equal y coord, x-intercept is bisector of foucs point - handled in ComputePolynomialZeros (a = 0)
    auto a = p1.a-p2.a;
    auto b = p1.b-p2.b;
    auto c = p1.c-p2.c;
    auto result = ComputePolynomialZeros(a,b,c);
    if(!result.has_value())
      return std::nullopt;
    auto& [x1,x2] = result.value();
    auto y1 = p1.GetY(x1);
    auto y2 = p1.GetY(x2);
    SPG_ASSERT(Equal(y1, p2.GetY(x1)));
    SPG_ASSERT(Equal(y2, p2.GetY(x2)));
    auto points = std::pair(Point2d((float)x1,y1), Point2d((float)x2,y2));
    return std::optional(points); 
  }

#if 0
  static bool ValidZero(Parabola const& parab_left, Parabola const& parab_right, Point2d const& zero)
  {
    auto eps = 1e-7;
    auto yl_left =  parab_left.GetY(zero.x - eps);
    auto yr_left =  parab_right.GetY(zero.x - eps);
    auto yl_right = parab_left.GetY(zero.x + eps);
    auto yr_right = parab_right.GetY(zero.x + eps);
    return (yl_left > yr_left) && (yl_right < yr_right);
  } 
#endif

  //TODO: this should be a member of Voronoi class
  static bool IsValidBreakpoint(Parabola const& arc_left, Parabola const& arc_right, float break_x) 
  {
    //todo - check case of degenerate arc

    auto eps = 1e-4;
    auto yl_arc_left = arc_left.GetY(break_x - eps);
    auto yr_arc_left = arc_left.GetY(break_x + eps);

    auto yl_arc_right = arc_right.GetY(break_x - eps);
    auto yr_arc_right = arc_right.GetY(break_x + eps);

    return (yl_arc_left <= yl_arc_right) && ( yr_arc_left >= yr_arc_right);
  }

#if (VORONOI_VERS==1)

  bool Voronoi::EventCompare::operator()(const Event& a, const Event& b) const noexcept
  {
    return std::visit([](const auto& lhs, const auto& rhs) {
      // greater_than puts smallest y val at top
      if(lhs.GetPoint().y > rhs.GetPoint().y)
        return true;
      if(rhs.GetPoint().y > lhs.GetPoint().y)  
        return false;
      return lhs.GetPoint().x > rhs.GetPoint().x;

    //same as: above (tie takes lvalues, so need to do this, or else return a const ref
    #if 0
      auto y_left = lhs.GetPoint().y;
      auto y_right = rhs.GetPoint().y;
      auto x_left = lhs.GetPoint().x;
      auto x_right = lhs.GetPoint().x;

      return std::tie(y_left, x_left) >
            std::tie(y_right, x_right);
    #endif
    }, a, b);
  }

  bool Voronoi::NodeCompare::CompArcToArc(Node const & arc1,  Node const& arc2) const noexcept
  {
    //Todo - this situation should not happen - if it does then it means that an arc will not be a leaf.
    SPG_ERROR("Arc-Arc comp ID:{} to ID{}", arc1.unique_id, arc2.unique_id);
    // return true if arc1 is to the left of arc2
    SPG_ASSERT((arc1.type == Node::Type::Leaf) && (arc2.type == Node::Type::Leaf));
    auto const& beach = ctx->m_beach;
    auto const y_sweep = ctx->m_sweep;
    if(arc1.break_right.IsNil()) {
      SPG_ASSERT(!arc2.break_right.IsNil())
      return false; //arc1 is at extreme right
    }
    if(arc2.break_right.IsNil()) {
      SPG_ASSERT(!arc1.break_right.IsNil())
      return true; //arc2 is at extreme right
    }
    auto bp_r_arc1 = arc1.break_right.Val();
    auto bp_r_arc2 = arc2.break_right.Val();
    SPG_ASSERT(bp_r_arc1.type == Node::Type::Internal);
    SPG_ASSERT(bp_r_arc2.type == Node::Type::Internal);
    auto x1 = ComputeBreakpointX(bp_r_arc1,y_sweep);
    auto x2 = ComputeBreakpointX(bp_r_arc2,y_sweep);
    if(Equal(x1,x2)) { 
      // *should* only get this from a new site event - new arc is initially degenerate in which case the unique_id (node counter) specifies the geometrical ordering of the nodes)
      return bp_r_arc1.unique_id < bp_r_arc2.unique_id;
    }
    return x1<x2;
  }

  bool Voronoi::NodeCompare::CompArcToBP(Node const & arc,  Node const& bp) const noexcept
  {
    // return true if arc is to the left of bp
    SPG_ASSERT((arc.type == Node::Type::Leaf) && (bp.type == Node::Type::Internal));
    auto const& beach = ctx->m_beach;
    auto const y_sweep = ctx->m_sweep;
    if(arc.break_right.IsNil()) {
      return false; //arc is at extreme right
    }
    auto bp_r_arc = arc.break_right.Val();
    auto x_arc = ComputeBreakpointX(bp_r_arc,y_sweep);
    auto x_bp = ComputeBreakpointX(bp,y_sweep);
    if(Equal(x_arc,x_bp))
      return true; //arcs right bp is this bp => arc is to the left
    return x_arc < x_bp;
  }

  bool Voronoi::NodeCompare::CompBPToBP(Node const & bp1,  Node const& bp2) const noexcept
  {
    // return true if bp1 is to the left of bp2
    SPG_ASSERT((bp1.type == Node::Type::Internal) && (bp2.type == Node::Type::Internal));
    auto const& beach = ctx->m_beach;
    auto y_sweep = ctx->m_sweep;
    auto x1 = ComputeBreakpointX(bp1,y_sweep);
    auto x2 = ComputeBreakpointX(bp2,y_sweep);
    if(Equal(x1,x2))
      return bp1.unique_id < bp2.unique_id; //* Occurs for new site events
    return x1 < x2;  
  }

  bool Voronoi::NodeCompare::operator ()(Node const& n1,  Node const& n2) const noexcept
  {
    // Return true if n1 is to the left of n2
    if((n1.type == Node::Type::Leaf) && (n2.type == Node::Type::Leaf)) {
      return CompArcToArc(n1,n2);
    }
    else if((n1.type == Node::Type::Internal) && (n2.type == Node::Type::Internal)) {
      return CompBPToBP(n1,n2);
    }
    else if((n1.type == Node::Type::Leaf) && (n2.type == Node::Type::Internal)) {
      return CompArcToBP(n1,n2);
    }
    else if((n1.type == Node::Type::Internal) && (n2.type == Node::Type::Leaf)){
      return !CompArcToBP(n2,n1);
    }
    else {
      SPG_ASSERT(false);
      return false;
    }
      
  }

  
  float Voronoi::ComputeBreakpointX(const Node& node, float y_sweep)
  {
    SPG_ASSERT(node.type == Node::Type::Internal);
    Parabola arc_left{node.site_left, y_sweep};
    Parabola arc_right{node.site_right, y_sweep};

    if(arc_left.IsDegenerate()) { 
      // y_sweep == node.site_left.y => parab is a vertical line
      return node.site_left.x;
    }
    if(arc_right.IsDegenerate()) { 
      // y_sweep == node.site_right.y => parab is a vertical line
      return node.site_right.x;
    }
    auto res = ComputeIntersections(arc_left,arc_right);
    SPG_ASSERT(res.has_value());
    auto& [pnt1,pnt2] = res.value();
    if(Equal(pnt1,pnt2)) {
      //1 intersection only => y-coords of sites are equal.
      return pnt1.x; 
    }
    bool pnt1_valid = IsValidBreakpoint(arc_left,arc_right,pnt1.x);
    bool pnt2_valid = IsValidBreakpoint(arc_left,arc_right,pnt2.x);

    if(pnt1_valid && !pnt2_valid)
      return pnt1.x;
    if(pnt2_valid && !pnt1_valid)  
      return pnt2.x; // Always this one!
    //Todo: Both valid or both invalid => nurmerical issue - need a fallback test
    // If both pass, pick the root closest to previous root
    // If neither pass, pick the root that lies between left neighbour BP.x and right neighbour BP.x
    SPG_ASSERT(false);
    return pnt1.x;   
  }

  void Voronoi::GetArcTriples(const Node& arc) 
  {
    SPG_ASSERT(arc.type == Node::Type::Leaf);

  }

  Voronoi::Voronoi(std::vector<Geom::Point2d>& points)
  {

  }

  void Voronoi::Build(std::vector<Geom::Point2d>& points)
  {
    for(auto& p : points) {
      m_event_queue.push(SiteEvent{p});
    }

    while(!m_event_queue.empty()){
      auto e = m_event_queue.top();
      if(std::holds_alternative<SiteEvent>(e)) {
        HandleSiteEvent(std::get<SiteEvent>(e));
      } 
      else {
        HandleCircleEvent(std::get<CircleEvent>(e));
      }
      m_event_queue.pop();
    }
  }

  void Voronoi::HandleSiteEvent(SiteEvent e)
  {
    //Update sweepline
    m_sweep = e.site.y - 0.001f; //Prevent degenerate arcs 
    //Todo - doesn't work without moving sweep down slightly!
    //m_sweep = e.site.y;
    auto y_sweep{m_sweep};
    
    
    //* STEP 1. If beach tree (T) empty, insert event point into T
    if(m_beach.Empty()) {
      auto node = Node{};
      node.type = Node::Type::Leaf;
      node.site = e.site;
      node.unique_id = NextId();
      node.break_left = m_beach.GetNilNodeHandle();
      node.break_right = m_beach.GetNilNodeHandle();
      node.ctx = this;
      m_beach.Insert(node);
      return;
    }

    //* STEP 2. Search T for the arc vertically above event point.  If the leaf representing the arc points to a circle event in Q, the circle event is false alarm, delete it from Q
    auto itr = m_beach.Root();
    while((itr != m_beach.end()) && (itr->type != Node::Type::Leaf)) {
      auto& node = *itr;
      auto p_left = Parabola{node.site_left, y_sweep};
      auto p_right = Parabola{node.site_right, y_sweep};
      auto break_point_x = ComputeBreakpointX(node,y_sweep);
      if(e.site.x > break_point_x)
        itr = m_beach.RightChild(itr);
      else
        itr = m_beach.LeftChild(itr);  
    }
    SPG_ASSERT(itr != m_beach.end());
    auto& arc_orig = *itr;
    if(arc_orig.circle_event != nullptr) {
      arc_orig.circle_event->false_alarm = true;
    }

    //* STEP 3. Replace this leaf with a subtree having 3 leaves. Middle leaf stores new site event e.  Left and right leaves store site that was found.  Store 2 new site pairs as internal nodes

    //2 new breakpoints (internal nodes)
    // site_left defines the parabola arc left of the breakpoint
    // site_right defines the parabola arc right of the breakpoint
    Node bp_left,bp_right;
    bp_left.type = bp_right.type = Node::Type::Internal;
    bp_left.site_left = arc_orig.site;
    bp_left.site_right = e.site;
    bp_right.site_left = e.site;
    bp_right.site_right = arc_orig.site;

    //3 new arcs (leaf nodes)
    Node arc_left, arc_mid, arc_right; 
    arc_left.type = arc_mid.type = arc_right.type = Node::Type::Leaf;
    arc_left.site = arc_orig.site;
    arc_right.site = arc_orig.site;
    arc_mid.site = e.site;

    //Define the geometrical ordering of the 3 arcs and 2 breakpoints
    arc_left.unique_id = NextId();
    bp_left.unique_id = NextId();
    arc_mid.unique_id = NextId();
    bp_right.unique_id = NextId();
    arc_right.unique_id = NextId();
    arc_left.ctx = arc_mid.ctx = arc_right.ctx = bp_left.ctx=bp_right.ctx = this;

    auto bpl_node = m_beach.CreateNodeHandle(bp_left);
    auto bpr_node = m_beach.CreateNodeHandle(bp_right);
    auto arcl_node = m_beach.CreateNodeHandle(arc_left);
    auto arcm_node = m_beach.CreateNodeHandle(arc_mid);
    auto arcr_node = m_beach.CreateNodeHandle(arc_right);

    arc_left.break_left = arc_orig.break_left;
    arc_left.break_right = bpl_node;

    arc_mid.break_left = bpl_node;
    arc_mid.break_right = bpr_node;

    arc_right.break_left = bpr_node;
    arc_right.break_right = arc_orig.break_right;

    bp_left.arc_left = arcl_node;
    bp_left.arc_right = arcm_node;

    bp_right.arc_left = arcm_node;
    bp_right.arc_right = arcr_node;

    m_beach.SetNodeHandle(bpl_node,bp_left);
    m_beach.SetNodeHandle(bpr_node,bp_right);
    m_beach.SetNodeHandle(arcl_node,arc_left);
    m_beach.SetNodeHandle(arcm_node,arc_mid);
    m_beach.SetNodeHandle(arcr_node,arc_right);

    // Check if the arc to be erased has left/right BP's.  If so, the NodeHandles for these need to be updated.
    if(!arc_orig.break_left.IsNil()) {
      auto& bp = arc_orig.break_left.Val();
      bp.arc_right = arcl_node;
    }
     if(!arc_orig.break_right.IsNil()) {
      auto& bp = arc_orig.break_right.Val();
      bp.arc_left = arcr_node;
    }

    //replace original arc with the 5 new nodes
    m_beach.Erase(itr);
    auto bpl_it = m_beach.InsertNodeHandle(bpl_node);
    auto bpr_it = m_beach.InsertNodeHandle(bpr_node);
    auto arcl_it = m_beach.InsertNodeHandle(arcl_node);
    auto arcm_it = m_beach.InsertNodeHandle(arcm_node);
    auto arcr_it = m_beach.InsertNodeHandle(arcr_node);
    
     //* STEP 4.  Create new half edge records in diagram for the edge between bpl and bpr

     //* STEP 5.  Check the triple of consecutive arcs where the new arc (arcm) is the left most arc of the triple 

#if 1
    //New arc (arcm_node) is the left arc
    BeachItr bp_left_itr = Geom::Next(arcm_it,1);
    BeachItr bp_right_itr = Geom::Next(bp_left_itr,2);
    if(bp_left_itr != m_beach.end() && bp_right_itr != m_beach.end()) {
      auto bp_left = *bp_left_itr;
      auto bp_right = *bp_right_itr;
      SPG_INFO("BP's for triple arcs where new arc is on the left:")
      SPG_TRACE("Arc New:  {}", *arcm_it);
      SPG_TRACE("BP left:  {}", bp_left);
      SPG_TRACE("BP right: {}", bp_right);
      
       //*Add circle event into Queue, set pointers
      Line2d bisector1 = GetBisector(bp_left.site_left,bp_left.site_right);
      Line2d bisector2 = GetBisector(bp_right.site_left,bp_right.site_right);
      // intersection of bisectors *should* be the center of the circle with points defining the arc triple on its boundary
      Point2d q = ComputeIntersection(bisector1,bisector2);
      float radius = glm::length(q-bp_left.site_left);
      
      //check
      glm::vec2 p2_q = q-bp_left.site_right;
      glm::vec2 p3_q = q-bp_right.site_right;
      SPG_ASSERT(Equal(radius, glm::length(p2_q)) ) ;
      SPG_ASSERT(Equal(radius, glm::length(p3_q)) ) ;
      
      float y_circle = q.y - radius;
      //If the sweep line is above y-circle, *should* mean that BP's are converging => valid circle event
      if(m_sweep > y_circle) {
        auto p = Point2d{q.x,y_circle};
        CircleEvent e{p};
        e.leaf_going = bp_left.arc_right;
        m_event_queue.push(e);
      }

    } else {
      SPG_INFO("No arc triplet for new arc on left")
    }
    //New arc (arcm_node) is the right arc
    //Almost a duplicate of the case above! might want a refactor!
    bp_right_itr = Geom::Prev(arcm_it,1);
    bp_left_itr = Geom::Prev(bp_right_itr,2);
    if(bp_left_itr != m_beach.end() && bp_right_itr != m_beach.end()) {
      auto bp_left = *bp_left_itr;
      auto bp_right = *bp_right_itr;
      SPG_INFO("BP's for triple arcs where new arc is on the right:")
      SPG_TRACE("Arc New:  {}", *arcm_it);
      SPG_TRACE("BP left:  {}", bp_left);
      SPG_TRACE("BP right: {}", bp_right);

      //*Add circle event into Queue, set pointers
      Line2d bisector1 = GetBisector(bp_left.site_left,bp_left.site_right);
      Line2d bisector2 = GetBisector(bp_right.site_left,bp_right.site_right);
      // intersection of bisectors *should* be the center of the circle with points defining the arc triple on its boundary
      Point2d q = ComputeIntersection(bisector1,bisector2);
      float radius = glm::length(q-bp_left.site_left);
      
      //check
      glm::vec2 p2_q = q-bp_left.site_right;
      glm::vec2 p3_q = q-bp_right.site_right;
      SPG_ASSERT(Equal(radius, glm::length(p2_q)) ) ;
      SPG_ASSERT(Equal(radius, glm::length(p3_q)) ) ;
      
      float y_circle = q.y - radius;
      //If the sweep line is above y-circle, *should* mean that BP's are converging => valid circle event
      if(m_sweep > y_circle) {
        auto p = Point2d{q.x,y_circle};
        CircleEvent e{p};
        e.leaf_going = bp_left.arc_right;
        m_event_queue.push(e);
      }
  
    } else {
      SPG_INFO("No arc triplet for new arc on right")
    }
#endif
  }

  void Voronoi::HandleCircleEvent(CircleEvent e)
  {

  }

  std::string Voronoi::Node::ToString(Node const & n)
  {
    SPG_ASSERT(n.ctx != nullptr);
    auto const& beach = n.ctx->m_beach;
    auto const  y_sweep = n.ctx->m_sweep;
    std::string s{};
    if(n.type == Node::Type::Leaf) {
      s = std::format("ARC:{}, S:({},{}) ", n.unique_id, n.site.x,n.site.y);

      if(n.break_left.IsNil()) 
        s += std::format(", BP_l:Nil");
      else {
        auto bp_left = n.break_left.Val();
        s += std::format(", BP_l:{} ",bp_left.unique_id);
      }

      if(n.break_right.IsNil()) 
        s += std::format(", BP_r:Nil");
      else {
        auto bp_right = n.break_right.Val();
        auto x_arc = ComputeBreakpointX(bp_right,y_sweep);
        s += std::format(", BP_r:{}, X:{}",bp_right.unique_id, x_arc);
      }
      
      if(Parabola(n.site, y_sweep).IsDegenerate())
          s += std::format(", Deg");
    }
    else {
      SPG_ASSERT(!n.arc_left.IsNil());
      SPG_ASSERT(!n.arc_right.IsNil());
      auto arc_l = n.arc_left.Val();
      auto arc_r = n.arc_right.Val();
      auto x_bp = ComputeBreakpointX(n,y_sweep);
      s = std::format("BP:{}, AL:{}, AR:{}, X:{}",n.unique_id,arc_l.unique_id,arc_r.unique_id,x_bp);
      // s = std::format("BP: ID:{}, L:({},{}), R:({},{}), X:{}", n.unique_id, n.site_left.x,n.site_left.y, n.site_right.x,n.site_right.y, x_bp);
    }
    return s;
  }

  //==========================================================================
  //Testing / validation
  //==========================================================================

  void Voronoi::PrintBeach()
  {
    std::vector<Node> elements_out;
    m_beach.InOrderTraverse(elements_out);

    SPG_WARN("Beach:")
    for(const auto& node : m_beach) {
      SPG_TRACE(node);
    }
  }   

  void Voronoi::Test()
  {
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("Voronoi V1 - Test");
    SPG_WARN("-------------------------------------------------------------------------");
    {
      Voronoi voronoi;
    
      //Add a bunch of random values
      const uint32_t NUM_VALS = 40;
      const float MIN_VAL = 0;
      const float MAX_VAL = 100;
      
      std::random_device rd;                         
      std::mt19937 mt(rd()); 
      std::uniform_real_distribution<float> fdist(MIN_VAL, MAX_VAL); 
      
      //add some site events
      for(int i=0; i< 10; i++) {
        Point2d p(fdist(mt),fdist(mt));
        voronoi.m_event_queue.push(SiteEvent{p});
      }

      //add some circle events
      for(int i=0; i< 10; i++) {
        Point2d p(fdist(mt),fdist(mt));
        voronoi.m_event_queue.push(CircleEvent{p});
      }

      while(!voronoi.m_event_queue.empty()) {
        auto e = voronoi.m_event_queue.top();
        if(std::holds_alternative<SiteEvent>(voronoi.m_event_queue.top())) {
          auto e = std::get<SiteEvent>(voronoi.m_event_queue.top());
          std::cout << std::format("Site: ({},{})", e.site.x, e.site.y) << "\n";
        }
        else {
          auto e = std::get<CircleEvent>(voronoi.m_event_queue.top());
          std::cout << std::format("Circle: ({},{})", e.circle_bottom.x, e.circle_bottom.y) << "\n";
        }
        voronoi.m_event_queue.pop();
      }

    }
    
    {
      SPG_WARN("PARABOLA INTERSECTIONS");
      Parabola p1{{4,2}, -1 };
      Parabola p2{{5.5,-0.99}, -1};

      Parabola p3{{-34,2}, -2 };
      Parabola p4{{-30,2}, -2};

      Parabola p5{{24,0.3f}, 0.2 };
      Parabola p6{{23.5,0.201f}, 0.2};

      // auto res = GetIntersection(p1,p2);
      auto res = ComputeIntersections(p2,p1);
      SPG_ASSERT(res.has_value());
      auto pts = res.value();
      SPG_TRACE("P1,P2 Intersect: {}, {})",pts.first, pts.second);

      res = ComputeIntersections(p3,p4);
      SPG_ASSERT(res.has_value());
      pts = res.value();
      SPG_TRACE("P3,P4 Intersect: {}, {})",pts.first, pts.second);

      res = ComputeIntersections(p5,p6);
      SPG_ASSERT(res.has_value());
      pts = res.value();
      SPG_TRACE("P5,P6 Intersect: {}, {})",pts.first, pts.second);  
    }

    {
      //Bisectors and their intersections
      auto p1 = Point2d(50,10);
      auto p2 = Point2d(54,9);
      auto p3 = Point2d(48,7);
      auto p4 = Point2d(47.3,5.5);
     

      Line2d bisector_13 = GetBisector(p1,p3);
      Line2d bisector_12 = GetBisector(p1,p2);
      Line2d bisector_34 = GetBisector(p3,p4);
      Point2d q1 = ComputeIntersection(bisector_13,bisector_12);
      Point2d q2 = ComputeIntersection(bisector_34,bisector_13);
      SPG_TRACE("Bisectors 13 and 12 Intersect at: {}",q1); 
      SPG_TRACE("Bisectors 34 and 13 Intersect at: {}",q2); 


      glm::vec2 p1_q = q1-p1;
      glm::vec2 p2_q = q1-p2;
      glm::vec2 p3_q = q1-p3;

      SPG_TRACE("Dist pt to intersect: {}, {}, {} ", glm::length(p1_q), glm::length(p2_q), glm::length(p3_q));
      SPG_TRACE("Circle y-low: {}", q1.y - glm::length(p1_q));

      //* If sweep line is already under the bottom of the circle it means the bisectors are diverging, arc is expanding, no circle event
     
    }


    {
      //Beach line
    #if 1
      SPG_WARN("BEACH LINE");
      Voronoi voronoi;
      std::vector<SiteEvent> events{{50,10},{54,9},{48,7},{47,6}};
      
     
      for(auto& e : events) {
        voronoi.HandleSiteEvent(e);
        voronoi.PrintBeach();
      }
      voronoi.PrintBeach();
      voronoi.m_beach.Validate();
    #endif

    }

  }

  #endif

  namespace Voronoi_V2
  {
    uint32_t Voronoi::BeachElement::next_id = 0;

    Voronoi::Voronoi(std::vector<Geom::Point2d>& points) {
    }

    void Voronoi::Build(std::vector<Geom::Point2d>& points) {
      for(auto& p : points) {
        Event e = SiteEvent(p);
        auto event_ptr = std::make_unique<Event>(e);
        m_event_queue.push(std::move(event_ptr));
      }

      while(!m_event_queue.empty()){
        auto& e = m_event_queue.top();
        if(std::holds_alternative<SiteEvent>(*e)) {
          HandleSiteEvent(std::get<SiteEvent>(*e));
        } 
        else {
          HandleCircleEvent(std::get<CircleEvent>(*e));
        }
        m_event_queue.pop();
      }
    }

    void Voronoi::HandleSiteEvent(SiteEvent e) {

      //Update sweepline
      //Todo - doesn't work without moving sweep down slightly!
      m_sweep = e.site.y - 0.001f; //Prevent degenerate arcs 
    
      //* STEP 1. If beach tree empty, insert event point into tree
      if(m_beach.Empty()) {
        BeachNode* arc_node = m_beach.MakeArcNode(e.site);
        m_beach.Insert(arc_node);
        return;
      }

      //* STEP 2. Search T for the arc vertically above event point.  If the leaf representing the arc points to a circle event in Q, the circle event is false alarm, delete it from Q
      BeachNode* arc_node_above = m_beach.FindArcVerticallyAbovePoint(e.site,this);
      Arc arc_above = arc_node_above->GetArc();
      if(arc_above.circle_event != nullptr) {
        arc_above.circle_event->false_alarm = true;
      }

      //Breakpoints to the left/right of the arc found vertically above the site point (may be nil)
      BeachNode* bp_left_node_above = arc_node_above->GetPrevNode();
      BeachNode* bp_right_node_above = arc_node_above->GetNextNode(); 

       //* STEP 3. Replace this leaf with a subtree having 3 leaves. Middle leaf stores new site event e.  Left and right leaves store site that was found.  Store 2 new site pairs as internal nodes
      //2 new breakpoints, 3 new arcs
      //Need to be in this order to that element_id increments left to right
      BeachNode* arc_node_left =  m_beach.MakeArcNode(arc_above.site);
      BeachNode* bp_node_left =   m_beach.MakeBreakpointNode(arc_above.site,e.site);
      BeachNode* arc_node_mid =   m_beach.MakeArcNode(e.site);
      BeachNode* bp_node_right =  m_beach.MakeBreakpointNode(e.site,arc_above.site);
      BeachNode* arc_node_right = m_beach.MakeArcNode(arc_above.site);

      // Set adj left, adj right (next,prev nodes) in each new BeachNode
      arc_node_left->SetPrevAndNextNodes(bp_left_node_above,bp_node_left);
      bp_node_left->SetPrevAndNextNodes(arc_node_left,arc_node_mid);
      arc_node_mid->SetPrevAndNextNodes(bp_node_left,bp_node_right);
      bp_node_right->SetPrevAndNextNodes(arc_node_mid,arc_node_right);
      arc_node_right->SetPrevAndNextNodes(bp_node_right,bp_right_node_above);

      // Check if the arc to be erased has left/right BP's.  If so, the left/right nodes for these breakpoints need to be updated.
      if(bp_left_node_above != m_beach.GetNilNode())
          bp_left_node_above->SetNextNode(arc_node_left);
      if(bp_right_node_above != m_beach.GetNilNode())
          bp_right_node_above->SetNextNode(arc_node_right);  

      //replace original arc with the 5 new nodes
      m_beach.Erase(arc_node_above);
      auto bpl_it = m_beach.Insert(bp_node_left);
      auto bpr_it = m_beach.Insert(bp_node_left);
      auto arcl_it = m_beach.Insert(arc_node_left);
      auto arcm_it = m_beach.Insert(arc_node_mid);
      auto arcr_it = m_beach.Insert(arc_node_right);  
        
       //* STEP 4.  Create new half edge records in diagram for the edge between bpl and bpr

      //* STEP 5.  Check the triple of consecutive arcs where the new arc (arcm) is the left most arc of the triple 


    }

    void Voronoi::HandleCircleEvent(CircleEvent e) {

    }

    //TODO: member of BeachTree class?
    float Voronoi::ComputeBreakpointX(const Breakpoint& bp) {
      Parabola arc_left{bp.site_left, m_sweep};
      Parabola arc_right{bp.site_right, m_sweep};

      if(arc_left.IsDegenerate()) { 
        // y_sweep == node.site_left.y => parab is a vertical line
        return bp.site_left.x;
      }
      if(arc_right.IsDegenerate()) { 
        // y_sweep == node.site_right.y => parab is a vertical line
        return bp.site_right.x;
      }
      auto res = ComputeIntersections(arc_left,arc_right);
      SPG_ASSERT(res.has_value());
      auto& [pnt1,pnt2] = res.value();
      if(Equal(pnt1,pnt2)) {
        //1 intersection only => y-coords of sites are equal.
        return pnt1.x; 
      }
      bool pnt1_valid = IsValidBreakpoint(arc_left,arc_right,pnt1.x);
      bool pnt2_valid = IsValidBreakpoint(arc_left,arc_right,pnt2.x);

      if(pnt1_valid && !pnt2_valid)
        return pnt1.x;
      if(pnt2_valid && !pnt1_valid)  
        return pnt2.x; // Always this one!
      //Todo: Both valid or both invalid => nurmerical issue - need a fallback test
      // If both pass, pick the root closest to previous root
      // If neither pass, pick the root that lies between left neighbour BP.x and right neighbour BP.x
      SPG_ASSERT(false);
      return pnt1.x;   
    }


    Voronoi::BeachNode* Voronoi::BeachTree::FindArcVerticallyAbovePoint(Point2d point,Voronoi* ctx) 
    {
      SPG_ASSERT(ctx != nullptr);
      BeachNode* node = m_root;
      while((node != m_nil) && (node->IsBreakpoint())) {
        Breakpoint bp = node->GetBreakpoint();
        auto p_left = Parabola{bp.site_left, ctx->m_sweep};
        auto p_right = Parabola{bp.site_right, ctx->m_sweep};
        float bp_x = ctx->ComputeBreakpointX(bp);
        if(point.x > bp_x)
          node = node->right;
        else
          node = node->left;
      }
      SPG_ASSERT(node != m_nil);
      SPG_ASSERT(node->IsArc());
      return node;
    }

#if 0
    bool Voronoi::BeachNodeComp::operator ()(BeachNode* node1,  BeachNode* node2) const noexcept {
      return false;
    }

    bool Voronoi::BeachNodeComp::CompArcToBP(BeachNode* arc_node,  BeachNode* bp_node) const noexcept {
     return false;
    }

    bool Voronoi::BeachNodeComp::CompBPToBP(BeachNode* bp1_node,  BeachNode* bp2_node) const noexcept {
      return false;
    }
#endif

    bool Voronoi::BeachElementComp::operator ()(BeachElement const & element1,  BeachElement const& element2) const noexcept {
      // Return true if element1 is to the left of element2
      if(element1.IsBreakpoint() && element2.IsBreakpoint() )
        return CompBPToBP(element1,element2);

      if(element1.IsArc() && element2.IsBreakpoint())
        return CompArcToBP(element1,element2);

      if(element2.IsArc() && element1.IsBreakpoint())
        return !CompArcToBP(element1,element1);  

      SPG_ASSERT(false);
      return false;
    }

    bool Voronoi::BeachElementComp::CompArcToBP(BeachElement const & arc_el,  BeachElement const& bp_el) const noexcept {
       // return true if arc is to the left of bp
      SPG_ASSERT(arc_el.IsArc());
      SPG_ASSERT(bp_el.IsBreakpoint());
      auto beach = ctx->m_beach;
     
      if(arc_el.next_node == beach.GetNilNode())
        return false; //arc is at extreme right

      SPG_ASSERT(arc_el.next_node->IsBreakpoint());
      Breakpoint arcs_right_bp = arc_el.next_node->GetBreakpoint();

      float arc_x = ctx->ComputeBreakpointX(arcs_right_bp);
      float bp_x = ctx->ComputeBreakpointX(bp_el.GetBreakpoint());
      if(Equal(arc_x,bp_x))
        return true; //arcs right bp is this bp => arc is to the left
      return arc_x < bp_x;
    }

    bool Voronoi::BeachElementComp::CompBPToBP(BeachElement const & bp1_el,  BeachElement const& bp2_el) const noexcept {
      // return true if bp1 is to the left of bp2
      SPG_ASSERT(bp1_el.IsBreakpoint());
      SPG_ASSERT(bp2_el.IsBreakpoint());
     
      float x1 = ctx->ComputeBreakpointX(bp1_el.GetBreakpoint());
      float x2 = ctx->ComputeBreakpointX(bp2_el.GetBreakpoint());
      if(Equal(x1,x2))
        return bp1_el.element_id < bp2_el.element_id; //* Occurs for new site events
      return x1 < x2;  
    }

    bool Voronoi::EventCompare::operator()(const EventPtr& a, const EventPtr& b) const noexcept
    {
      return std::visit([](const auto& lhs, const auto& rhs) {
        if(lhs.GetPoint().y < rhs.GetPoint().y)
          return true;
        if(rhs.GetPoint().y < lhs.GetPoint().y)  
          return false;
        return lhs.GetPoint().x < rhs.GetPoint().x;
      }, *a, *b);
    }

  
    std::string Voronoi::BeachElement::ToString(Voronoi* ctx) {
      SPG_ASSERT(ctx != nullptr);
      auto& beach = ctx->m_beach;
      std::string s{""};
      if(IsArc()) {
        Arc arc = GetArc();
        s = std::format("ARC:{}, S:({},{}) ",element_id, arc.site.x, arc.site.y);
        if(prev_node == beach.GetNilNode()) 
          s += std::format(", BP_l:Nil");
        else {
          SPG_ASSERT(prev_node->IsBreakpoint());
          BeachElement left_el = prev_node->GetElement();
          s += std::format(", BP_l:{} ",left_el.element_id);
        }
        if(next_node == beach.GetNilNode()) 
          s += std::format(", BP_r:Nil");
        else {
          SPG_ASSERT(next_node->IsBreakpoint());
          BeachElement right_el = next_node->GetElement();
          s += std::format(", BP_r:{} ",right_el.element_id);
        }
        if(Parabola(arc.site, ctx->m_sweep).IsDegenerate())
            s += std::format(", Deg");
      }
      else {
        SPG_ASSERT(IsBreakpoint());
        Breakpoint bp = GetBreakpoint();
        SPG_ASSERT(prev_node->IsArc());
        SPG_ASSERT(next_node->IsArc());
        BeachElement left_arc = prev_node->GetElement();
        BeachElement right_arc = next_node->GetElement();
        float bp_x = ctx->ComputeBreakpointX(bp);
        s = std::format("BP:{}, AL:{}, AR:{}, X:{}",element_id, left_arc.element_id,right_arc.element_id, bp_x);
      }
      return s;
    }

    void Voronoi::PrintBeach()
    {
      std::vector<BeachElement> elements_out;
      m_beach.InOrderTraverse(elements_out);

      SPG_WARN("Beach:")
      for(auto& element : m_beach) {
        SPG_TRACE("{}",element.ToString(this));
      }
    }   

    void Voronoi::Test() 
    {
      SPG_WARN("-------------------------------------------------------------------------");
      SPG_WARN("Voronoi V2 - Test");
      SPG_WARN("-------------------------------------------------------------------------");


       SPG_WARN("Contstructing Geom::Voronoi_V2::Voronoi!")
      Voronoi voronoi;

      //Event queue
      {
        SPG_WARN("Add a some of random events")
        const uint32_t NUM_VALS = 40;
        const float MIN_VAL = 0;
        const float MAX_VAL = 100;
        
        std::random_device rd;                         
        std::mt19937 mt(rd()); 
        std::uniform_real_distribution<float> fdist(MIN_VAL, MAX_VAL); 
        
        //add some site events
        for(int i=0; i< 10; i++) {
          Point2d p(fdist(mt),fdist(mt));
          Event e = SiteEvent(p);
          auto event_ptr = std::make_unique<Event>(e);
          voronoi.m_event_queue.push(std::move(event_ptr));
        }

        //add some circle events
        for(int i=0; i< 10; i++) {
          Point2d p(fdist(mt),fdist(mt));
          Event e = CircleEvent(p);
          auto evemt_ptr = std::make_unique<Event>(e);
          voronoi.m_event_queue.push(std::move(evemt_ptr));
        }

        //print
        SPG_INFO("Event Queue:")
        while(!voronoi.m_event_queue.empty()) {
          const auto& event_top = *(voronoi.m_event_queue.top());
          if(std::holds_alternative<SiteEvent>(event_top)) {
            auto& e = std::get<SiteEvent>(event_top);
            SPG_TRACE("Site: {}", e.site);
          }
          else {
            auto& e = std::get<CircleEvent>(event_top);
            SPG_TRACE("Circle: {}", e.circle_low);
          }
          voronoi.m_event_queue.pop();
        }
      }
      //-----------------------------------------------------------------
      {
        SPG_WARN("PARABOLA INTERSECTIONS");
        Parabola p1{{4,2}, -1 };
        Parabola p2{{5.5,-0.99}, -1};

        Parabola p3{{-34,2}, -2 };
        Parabola p4{{-30,2}, -2};

        Parabola p5{{24,0.3f}, 0.2 };
        Parabola p6{{23.5,0.201f}, 0.2};

        // auto res = GetIntersection(p1,p2);
        auto res = ComputeIntersections(p2,p1);
        SPG_ASSERT(res.has_value());
        auto pts = res.value();
        SPG_TRACE("P1,P2 Intersect: {}, {})",pts.first, pts.second);

        res = ComputeIntersections(p3,p4);
        SPG_ASSERT(res.has_value());
        pts = res.value();
        SPG_TRACE("P3,P4 Intersect: {}, {})",pts.first, pts.second);

        res = ComputeIntersections(p5,p6);
        SPG_ASSERT(res.has_value());
        pts = res.value();
        SPG_TRACE("P5,P6 Intersect: {}, {})",pts.first, pts.second);  
      }

      {
        SPG_WARN("Bisectors and their intersections");
        auto p1 = Point2d(50,10);
        auto p2 = Point2d(54,9);
        auto p3 = Point2d(48,7);
        auto p4 = Point2d(47.3,5.5);
      
        Line2d bisector_13 = GetBisector(p1,p3);
        Line2d bisector_12 = GetBisector(p1,p2);
        Line2d bisector_34 = GetBisector(p3,p4);
        Point2d q1 = ComputeIntersection(bisector_13,bisector_12);
        Point2d q2 = ComputeIntersection(bisector_34,bisector_13);
        SPG_TRACE("Bisectors 13 and 12 Intersect at: {}",q1); 
        SPG_TRACE("Bisectors 34 and 13 Intersect at: {}",q2); 

        glm::vec2 p1_q = q1-p1;
        glm::vec2 p2_q = q1-p2;
        glm::vec2 p3_q = q1-p3;

        SPG_TRACE("Dist pt to intersect: {}, {}, {} ", glm::length(p1_q), glm::length(p2_q), glm::length(p3_q));
        SPG_TRACE("Circle y-low: {}", q1.y - glm::length(p1_q));

        //* If sweep line is already under the bottom of the circle it means the bisectors are diverging, arc is expanding, no circle event
      
      }

    }
  }

}