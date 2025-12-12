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

  static float ComputeBreakpointX(Point2d const& left_site, Point2d const& right_site, float sweep_y) 
  {
    Parabola left_parab(left_site, sweep_y);
    Parabola right_parab(right_site, sweep_y);
    
    if(left_parab.IsDegenerate())
      return left_site.x;
    if(right_parab.IsDegenerate())
      return right_site.x;

    auto result = ComputeIntersections(left_parab,right_parab);
    SPG_ASSERT(result.has_value());
    auto& [point1,point2] = result.value();
    if(Equal(point1,point2))
      return point1.x;   //1 intersection only => y-coords of sites are equal.
    
    bool pnt1_valid = IsValidBreakpoint(left_parab,right_parab,point1.x);
    bool pnt2_valid = IsValidBreakpoint(left_parab,right_parab,point2.x);

    if(pnt1_valid && !pnt2_valid)
      return point1.x;
    if(pnt2_valid && !pnt1_valid)  
      return point2.x; //Todo: Always this one! Investigate
    
    SPG_ASSERT(false);
    return point1.x;     
  }

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
        BeachNode* arc_node = m_beach.MakeArcNode(e.site,this);
        m_beach.Insert(arc_node);
        return;
      }

      //* STEP 2. Search T for the arc vertically above event point.  If the leaf representing the arc points to a circle event in Q, the circle event is false alarm.
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
      //Need to be in this order so that element_id increments left to right
      BeachNode* arc_node_left =  m_beach.MakeArcNode(arc_above.site,this);
      BeachNode* bp_node_left =   m_beach.MakeBreakpointNode(arc_above.site,e.site,this);
      BeachNode* arc_node_mid =   m_beach.MakeArcNode(e.site,this);
      BeachNode* bp_node_right =  m_beach.MakeBreakpointNode(e.site,arc_above.site,this);
      BeachNode* arc_node_right = m_beach.MakeArcNode(arc_above.site,this);

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
      auto bpr_it = m_beach.Insert(bp_node_right);
      auto arcl_it = m_beach.Insert(arc_node_left);
      auto arcm_it = m_beach.Insert(arc_node_mid);
      auto arcr_it = m_beach.Insert(arc_node_right);  
        
      //* STEP 4.  Create new half edge records in diagram for the edge between bpl and bpr

      //* STEP 5.  For the triple of consecutive arcs where the new arc (arc_node_mid) is the left/right most arc of the triple, check if the pair of breakpoints between them are converging.  If so add circle event to queue.
      NodePair Breakpoint_pair = GetBreakpointPairleft(arc_node_mid);
      AddCircleEventIfRequired(Breakpoint_pair,arc_node_mid);
      Breakpoint_pair = GetBreakpointPairRight(arc_node_mid);
      AddCircleEventIfRequired(Breakpoint_pair,arc_node_mid);
 
    }

    void Voronoi::HandleCircleEvent(CircleEvent e) {
      if(e.false_alarm)
        return;

      SPG_ASSERT(e.arc_node_going != nullptr);
      
      //* STEP 1 
      BeachNode* bp_left = e.arc_node_going->GetPrevNode();
      BeachNode* bp_right = e.arc_node_going->GetNextNode();
      SPG_ASSERT(bp_left->IsBreakpoint());
      SPG_ASSERT(bp_right->IsBreakpoint());
      BeachNode* arc_left = bp_left->GetPrevNode();
      BeachNode* arc_right = bp_right->GetNextNode();
      SPG_ASSERT(arc_left->IsArc());
      SPG_ASSERT(arc_right->IsArc());

      //* Delete all circle events for arcs sharing the same site point as e.arc_node_going - (search to the left and right of e.arc_node_going)  There sould be at most only 1 other arc sharing the same site

      // bp_left and bp_right merge into a single breakpoint.  Keep bp_left, delete bp_right, update their neighbour's next, prev pointers.  
      //Bypass the 2 nodes to be deleted - e.arc_node_going, bp_right
      bp_left->SetNextNode(arc_right);
      arc_right->SetPrevNode(bp_left);

      m_beach.Erase(e.arc_node_going);  
    }

    Voronoi::NodePair Voronoi::GetBreakpointPairleft(BeachNode* arc_node) {
      Voronoi::NodePair bp_pair;
      bp_pair.left = arc_node->GetNextNode();
      bp_pair.right = bp_pair.left->GetNextNode()->GetNextNode();
      return bp_pair;
    }

    Voronoi::NodePair Voronoi::GetBreakpointPairRight(BeachNode* arc_node) {
      Voronoi::NodePair bp_pair;
      bp_pair.left = arc_node->GetPrevNode();
      bp_pair.right = bp_pair.left->GetPrevNode()->GetPrevNode();
      return bp_pair;
    }

    void Voronoi::AddCircleEventIfRequired(NodePair bp_pair,BeachNode* new_arc) {
      SPG_ASSERT(bp_pair.left != nullptr && bp_pair.right != nullptr);
      SPG_ASSERT(new_arc != nullptr);

      if(bp_pair.left == m_beach.GetNilNode() || bp_pair.right == m_beach.GetNilNode()) {
        SPG_INFO("No arc triplet for new arc on left")
        return;
      }

      SPG_INFO("BP's for arc where new arc is on the left:")
      SPG_TRACE("Arc New:  {}", new_arc->GetElement());
      SPG_TRACE("BP left:  {}", bp_pair.left->GetElement());
      SPG_TRACE("BP right:  {}", bp_pair.right->GetElement());

      //Add circle event into the queue, set back pointers
      Breakpoint bp_left = bp_pair.left->GetBreakpoint();
      Breakpoint bp_right = bp_pair.right->GetBreakpoint();
      Line2d bisector1 = GetBisector(bp_left.site_left,bp_left.site_right);
      Line2d bisector2 = GetBisector(bp_right.site_left,bp_right.site_right);
      Point2d q = ComputeIntersection(bisector1,bisector2); // The center of the circle with points defining arc triple on its boundary - check below:
      float radius = glm::length(q-bp_left.site_left);
      SPG_ASSERT(Equal(radius, glm::length(q-bp_left.site_right)) ) ;
      SPG_ASSERT(Equal(radius, glm::length(q-bp_right.site_right)) ) ;

      float y_circle = q.y - radius;
      //If the sweep line is above y-circle, *should* mean that BP's are converging => valid circle event
      if(m_sweep > y_circle) {
        BeachNode* arc_node_going = bp_pair.left->GetNextNode();
        SPG_ASSERT(arc_node_going->IsArc());
        auto p = Point2d{q.x,y_circle};
        CircleEvent e{p};
        e.arc_node_going = bp_pair.left->GetNextNode();
        auto event_ptr = std::make_unique<Event>(e);
        Arc& arc = arc_node_going->GetArc();
        arc.circle_event = std::get_if<CircleEvent>(event_ptr.get());
        SPG_ASSERT(arc.circle_event != nullptr);
        m_event_queue.push(std::move(event_ptr));
      }
    }

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

    bool Voronoi::BeachElementComp::operator ()(BeachElement const & element1,  BeachElement const& element2) const noexcept {
      // Return true if element1 is to the left of element2
      if(element1.IsBreakpoint() && element2.IsBreakpoint() )
        return CompBPToBP(element1,element2);

      if(element1.IsArc() && element2.IsBreakpoint())
        return CompArcToBP(element1,element2);

      if(element2.IsArc() && element1.IsBreakpoint())
        return !CompArcToBP(element2,element1);  

      SPG_ASSERT(false);
      return false;
    }

    bool Voronoi::BeachElementComp::CompArcToBP(BeachElement const & arc_el,  BeachElement const& bp_el) const noexcept {
       // return true if arc is to the left of bp
      SPG_ASSERT(arc_el.IsArc());
      SPG_ASSERT(bp_el.IsBreakpoint());
      auto& beach = ctx->m_beach;
     
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

    std::string Voronoi::BeachElement::ToString(BeachElement const & e) {

      SPG_ASSERT(e.ctx != nullptr);
      auto& beach = e.ctx->m_beach;
      std::string s{""};
      if(e.IsArc()) {
        Arc arc = e.GetArc();
        s = std::format("ARC:{}, S:({},{}) ",e.element_id, arc.site.x, arc.site.y);
        if(e.prev_node == beach.GetNilNode()) 
          s += std::format(", BP_l:Nil");
        else {
          SPG_ASSERT(e.prev_node->IsBreakpoint());
          BeachElement left_el = e.prev_node->GetElement();
          s += std::format(", BP_l:{} ",left_el.element_id);
        }
        if(e.next_node == beach.GetNilNode()) 
          s += std::format(", BP_r:Nil");
        else {
          SPG_ASSERT(e.next_node->IsBreakpoint());
          BeachElement right_el = e.next_node->GetElement();
          float arc_x = e.ctx->ComputeBreakpointX(right_el.GetBreakpoint());
          s += std::format(", BP_r:{}, X:{} ",right_el.element_id,arc_x);
        }
        if(Parabola(arc.site, e.ctx->m_sweep).IsDegenerate())
            s += std::format(", DGT");
      }
      else {
        SPG_ASSERT(e.IsBreakpoint());
        Breakpoint bp = e.GetBreakpoint();
        SPG_ASSERT(e.prev_node->IsArc());
        SPG_ASSERT(e.next_node->IsArc());
        BeachElement left_arc = e.prev_node->GetElement();
        BeachElement right_arc = e.next_node->GetElement();
        float bp_x = e.ctx->ComputeBreakpointX(bp);
        s = std::format("BP:{}, AL:{}, AR:{}, X:{}",e.element_id, left_arc.element_id,right_arc.element_id, bp_x);
      }
      return s;
    }

    void Voronoi::PrintBeach()
    {
      // std::vector<BeachElement> elements_out;
      // m_beach.InOrderTraverse(elements_out);
      SPG_WARN("Beach:")
      for(auto& element : m_beach) {
        SPG_TRACE("{}",element);
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
        SPG_WARN("EVENT QUEUE");
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
            SPG_TRACE("Circle: {}", e.circle_point);
          }
          voronoi.m_event_queue.pop();
        }
      }
      
      //Parabola intersections
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

      //Bisectors and intersections
      {
        SPG_WARN("BISECTORS");
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

      //Beach line
      {
        SPG_WARN("BEACH LINE");
        Voronoi voronoi;
        std::vector<SiteEvent> events{{50,10},{54,9},{48,7},{47.3,5.5}};
        for( auto& e : events) {
          voronoi.HandleSiteEvent(e);
          voronoi.PrintBeach();
        }
        voronoi.m_beach.Validate();
      }
    }
  }

  namespace Voronoi_V3
  {
    uint32_t BeachElement::next_id = 0;

    Voronoi::Voronoi(std::vector<Point2d> points) : m_points{std::move(points)} {
      m_event_queue.Initialize(m_points);
      //m_beach.SetComparator(BeachNodeComp(this));
      m_beach.ctx = this;
      //Construct();
    }

    void Voronoi::Construct() {

      while(!m_event_queue.IsEmpty()) {
        Event* e = m_event_queue.Pop();
        if(e->type == Event::Type::Site )
          HandleSiteEvent(e);
        else {
          SPG_ASSERT(e->type == Event::Type::Circle);
          HandleCircleEvent(e);
        }
        //Delete event (since it was heap allocated)
        if(e->type == Event::Type::Circle ) {
          delete e->point; // this point was heap allocated
          e->point = nullptr;
        }
        delete e;
        e = nullptr;
      }
    }

    void Voronoi::HandleSiteEvent(Event* e) {
      // Update sweepline
      m_sweep = e->point->y - 0.001f; //Prevent degenerate arcs 

      //* STEP 1.
      if(m_beach.Empty()) {
        auto* arc_node = m_beach.MakeArcNode(e->point);
        m_beach.Insert(arc_node);
        return;
      }
      //* STEP 2
      auto* arc_node_above = m_beach.FindArcNodeAbove(e->point, m_sweep);
      m_beach.SetCircleEventValidity(arc_node_above,false);

      //* STEP 3
      auto arc_triple = m_beach.MakeArcTriple(e, arc_node_above, m_sweep);
      m_beach.InsertArcTriple(e, arc_triple, arc_node_above); //Also deletes arc_node_above first
      
      //* STEP 4

      //* STEP 5
      Arc* arc_new = m_beach.GetArc(arc_triple[2]);
      BreakpointPair bp_left_pair = m_beach.GetLeftBreakpointPair(arc_new);
      BreakpointPair bp_right_pair = m_beach.GetRightBreakpointPair(arc_new);
      InsertCircleEventIfNeeded(bp_left_pair,arc_new); //second param only needed for debug/validation
      InsertCircleEventIfNeeded(bp_right_pair,arc_new); //second param only needed for debug/validation
    }

    void Voronoi::HandleCircleEvent(Event* e) {
      SPG_INFO("Circle event to be processed!");
      SPG_ASSERT(e != nullptr);
      if(!e->valid)
        return;
      SPG_ASSERT(e->diappearing_arc != nullptr);

      //* STEP 1 - delete the disappearing arc
      


    }

    void Voronoi::InsertCircleEventIfNeeded(BreakpointPair bp_pair, Arc* new_arc) {
      if(bp_pair.left == nullptr || bp_pair.right == nullptr) {
         SPG_INFO("No arc triplet")
          return;
      }
      SPG_ASSERT(bp_pair.left->right_arc == bp_pair.right->left_arc);
      SPG_INFO("Breakpoints adjacent to new arc:")
      SPG_TRACE("Arc New:  {}", *new_arc->element);
      SPG_TRACE("BP left:  {}", *bp_pair.left->element);
      SPG_TRACE("BP right:  {}", *bp_pair.right->element);

      //Breakpoint* bp_left = bp_pair.left;
      Line2d bisector_left = GetBisector(*(bp_pair.left->left_arc->site), *(bp_pair.left->right_arc->site));
      Line2d bisector_right = GetBisector(*(bp_pair.right->left_arc->site), *(bp_pair.right->right_arc->site));

      //q = The center of the circle with points defining arc triple on its boundary - 
      Point2d q = ComputeIntersection(bisector_left,bisector_right); 
      float radius = glm::length(q-*(bp_pair.left->left_arc->site));

      //validate:
      SPG_ASSERT(Equal(radius, glm::length(q-*(bp_pair.left->right_arc->site))) ) ;
      SPG_ASSERT(Equal(radius, glm::length(q-*(bp_pair.right->right_arc->site))) ) ;

      //Add a circle event if the breakpoints are converging.
      //If the sweep line is above circle bottom, *should* mean that BP's are converging => valid circle event
      float circle_bottom = q.y - radius;
      if(m_sweep > circle_bottom) {
        SPG_INFO("Breakpoints converging - Adding circle event")
        Event* circle_event = new Event;
        circle_event->type = Event::Type::Circle;
        circle_event->point = new Point2d(q.x, circle_bottom);
        circle_event->diappearing_arc = bp_pair.left->right_arc;
        circle_event->circle = {q,radius};
        circle_event->valid = true;
        circle_event->diappearing_arc->circle_event = circle_event;
        m_event_queue.Push(circle_event);
      }
      else {
        SPG_INFO("Breakpoints NOT converging - NOT Adding circle event")
      }
    }

    void Voronoi::PrintBeach() {
      SPG_WARN("Beach: (Via TreeTraversal)")
      for(auto& element : m_beach) {
        SPG_TRACE("{}",element);
      }
  #if 1
      SPG_WARN("Beach: (Via Following internal links)")
      auto first = m_beach.begin();
      auto* node = first.Get();
      BeachElement* el = &(node->value);
      SPG_TRACE("{}",*el);
      while(true) {
         if(el->is_arc) {
            Breakpoint* bp_right = el->arc->right_bp;
            if(bp_right == nullptr)
              break;
            el = bp_right->element;
            SPG_TRACE("{}",*el);
         }
         else {
          Arc* arc_right = el->breakpoint->right_arc;
          if(arc_right == nullptr)
            break;
          el = arc_right->element;
            SPG_TRACE("{}",*el);  
         }
      }
  #endif
  }

    void EventQueue::Initialize(std::vector<Point2d> const& points) {
      for(auto& p : points) {
        Event* e = new Event;
        e->type = Event::Type::Site;
        e->point = &p;
        Push(e);
      }
    }

    bool BeachElementComp::operator () (BeachElement const& el1, BeachElement const& el2) const {
      // Return true if element1 is to the left of element2
      if(el1.is_arc && el2.is_arc)
        return CompArcToArc(el1,el2);
      if(!el1.is_arc && !el2.is_arc)
        return CompBPToBP(el1,el2);
      if(el1.is_arc && !el2.is_arc)
        return CompArcToBP(el1,el2);   
      if(el2.is_arc && !el1.is_arc)
        return !CompArcToBP(el2,el1);   
      SPG_ASSERT(false);
      return false;
    }

    bool BeachElementComp::CompArcToArc(BeachElement const& arc1,  BeachElement const& arc2) const {
      // return true if arc1 is to the left of arc2
      if(arc1.arc->left_bp == nullptr || arc2.arc->right_bp == nullptr)
        return true;
      if(arc1.arc->right_bp == nullptr || arc2.arc->left_bp == nullptr)
        return false; 
      float sweep_y = ctx->GetSweepY();
      float x1 = arc1.arc->right_bp->CurrentX(sweep_y);
      float x2 = arc2.arc->right_bp->CurrentX(sweep_y);
      if(Equal(x1,x2)) {
        SPG_ASSERT(false); //Should never happen since sweep line nudged sown slightly to prevent this
        return arc1.arc->id < arc2.arc->id; // Occurs for new site events if sweep line not moved down
      }
      return x1 < x2;  
    }

    bool BeachElementComp::CompArcToBP(BeachElement const & arc_el,  BeachElement const& bp_el) const {
      // return true if arc is to the left of bp
      if(arc_el.arc->left_bp == nullptr)
        return true; //arc is at extreme left of beachline
      if(arc_el.arc->right_bp == nullptr)
        return false; //arc is at extreme right of beachline
      float sweep_y = ctx->GetSweepY();
      Breakpoint* bp_right = arc_el.arc->right_bp;
      float arc_x = bp_right->CurrentX(sweep_y);
      float bp_x = bp_el.breakpoint->CurrentX(sweep_y);
      if(Equal(arc_x,bp_x))
        return true; //arcs right bp is this bp => arc is to the left
      return arc_x < bp_x;
    }

    bool BeachElementComp::CompBPToBP(BeachElement const & bp1,  BeachElement const& bp2) const {
      // return true if bp1 is to the left of bp2
      float x1 = bp1.breakpoint->CurrentX(ctx->GetSweepY());
      float x2 = bp2.breakpoint->CurrentX(ctx->GetSweepY());
      if(Equal(x1,x2)) {
        SPG_ASSERT(false); //Should never happen since sweep line nudged sown slightly to prevent this
        return bp1.breakpoint->id < bp2.breakpoint->id; // Occurs for new site events if sweep line not moved down
      }
      return x1 < x2;  
    }

    BeachTree::BeachNode* BeachTree::MakeArcNode(Point2d const * site) {
      SPG_ASSERT(site != nullptr);
      BeachElement el;
      el.arc = new Arc; 
      el.arc->site = site;
      el.arc->id = BeachElement::next_id++;
      el.is_arc = true;
      el.ctx = this->ctx; //only for printing
      BeachNode* node = this->MakeNode(el,m_nil);
      node->value.arc->element = &(node->value); //only for printing beachline!
      return node;
    }

    BeachTree::BeachNode* BeachTree::MakeBreakpointNode() {
      BeachElement el;
      el.breakpoint = new Breakpoint;
      el.breakpoint->id = BeachElement::next_id++;
      el.is_arc = false;
      el.ctx = this->ctx; //only for printing
      BeachNode* node = this->MakeNode(el,m_nil);
      node->value.breakpoint->element = &(node->value); //only for printing beachline!
      return node;
    } 

    BeachTree::BeachNode* BeachTree::MakeBreakpointNode(Arc* left_arc, Arc* right_arc) {
      SPG_ASSERT(left_arc != nullptr && right_arc != nullptr);    
      BeachElement el;
      el.breakpoint = new Breakpoint;
      el.breakpoint->left_arc = left_arc;
      el.breakpoint->right_arc = right_arc;
      el.breakpoint->id = BeachElement::next_id++;
      el.is_arc = false;
      el.ctx = this->ctx; //only for printing
      BeachNode* node = this->MakeNode(el,m_nil);
      node->value.breakpoint->element = &(node->value); //only for printing beachline!
      return node;
    }

     BeachTree::BeachNode* BeachTree::FindArcNodeAbove(Point2d const * site, float sweep_y) {
      SPG_ASSERT(site != nullptr);
      auto* tree_node = m_root;
      while(tree_node != m_nil && !tree_node->value.is_arc) {
        Breakpoint* breakpoint = tree_node->value.breakpoint;
        float bp_x = breakpoint->CurrentX(sweep_y);
        if(site->x > bp_x)
          tree_node = tree_node->right;
        else
          tree_node = tree_node->left;
      }
      SPG_ASSERT(tree_node != m_nil);
      SPG_ASSERT(tree_node->value.is_arc);
      return tree_node;
    }

    BeachTree::ArcTriple BeachTree::MakeArcTriple(Event* site_event, BeachNode* replaced_arc_node, float y_sweep) {
      ArcTriple arc_triple; 
      //BeachNode* arc_node_above = FindArcNodeAbove(site_event.site, y_sweep);
      SPG_ASSERT(replaced_arc_node != nullptr);
      Arc* replaced_arc = GetArc(replaced_arc_node);
      SPG_ASSERT(replaced_arc != nullptr);
      arc_triple[0] = MakeArcNode(replaced_arc->site);
      arc_triple[1] = MakeBreakpointNode();
      arc_triple[2] = MakeArcNode(site_event->point);
      arc_triple[3] = MakeBreakpointNode();
      arc_triple[4] = MakeArcNode(replaced_arc->site);
      // arc_triple[1] = MakeBreakpointNode(GetArc(arc_triple[0]),GetArc(arc_triple[2]));
      // arc_triple[3] = MakeBreakpointNode(GetArc(arc_triple[2]),GetArc(arc_triple[4]));
      SetArcNeighbours(arc_triple[0], LeftBreakpoint(replaced_arc_node), GetBreakpoint(arc_triple[1]));
      SetArcNeighbours(arc_triple[2], GetBreakpoint(arc_triple[1]), GetBreakpoint(arc_triple[3]));
      SetArcNeighbours(arc_triple[4], GetBreakpoint(arc_triple[3]), RightBreakpoint(replaced_arc_node));
      SetBreakpointNeighbours(arc_triple[1], GetArc(arc_triple[0]),GetArc(arc_triple[2]));
      SetBreakpointNeighbours(arc_triple[3], GetArc(arc_triple[2]),GetArc(arc_triple[4]));

      //chk
      //Todo - precalculate x-vals for each new element so they they don't need to be recalculated multiple times when inserted
      float sweep_y = ctx->GetSweepY();
      float x_bp1 = arc_triple[1]->value.breakpoint->CurrentX(sweep_y);
      float x_bp2 = arc_triple[3]->value.breakpoint->CurrentX(sweep_y);

      return arc_triple;
    }

    void BeachTree::InsertArcTriple(Event* site_event, ArcTriple& arc_triple, BeachNode* replaced_arc_node) {

      // Check if arc_replaced has left/right BP's.  If so, the left/right nodes for these breakpoints need to be updated.
      Breakpoint* far_left_bp = LeftBreakpoint(replaced_arc_node);
      Breakpoint* far_right_bp = RightBreakpoint(replaced_arc_node);
      if(far_left_bp != nullptr)
        far_left_bp->right_arc = GetArc(arc_triple[0]);
      if(far_right_bp != nullptr)
        far_right_bp->left_arc = GetArc(arc_triple[4]);  
      // Delete Arc found above (was heap allocated)
      delete replaced_arc_node->value.arc; 
      replaced_arc_node->value.arc = nullptr;
      //Erase from tree
      this->Erase(replaced_arc_node);
      for(auto* node : arc_triple)
        this->Insert(node);
      // Hope for the best
    }

    BreakpointPair BeachTree::GetLeftBreakpointPair(Arc* arc) {
      SPG_ASSERT(arc != nullptr)
      BreakpointPair bp_pair;
      bp_pair.right = arc->left_bp; 
      if(arc->left_bp != nullptr)
        bp_pair.left = arc->left_bp->left_arc->left_bp;
      return bp_pair;  
    }

    BreakpointPair BeachTree::GetRightBreakpointPair(Arc* arc) {
      SPG_ASSERT(arc != nullptr)
      BreakpointPair bp_pair;
      bp_pair.left = arc->right_bp;
      if(arc->right_bp != nullptr)
        bp_pair.right = arc->right_bp->right_arc->right_bp;
      return bp_pair;  
    }

    bool BeachTree::IsArc(BeachNode* node) {
      SPG_ASSERT(node != nullptr);
      return node->value.is_arc;
    }

    bool BeachTree::IsBreakpoint(BeachNode* node) {
      SPG_ASSERT(node != nullptr);
      return !(node->value.is_arc);
    }

    Arc* BeachTree::GetArc(BeachNode* arc_node) {
      return arc_node->value.arc;
    }

    Breakpoint* BeachTree::GetBreakpoint(BeachNode* bp_node) {
      return bp_node->value.breakpoint;
    }

    void BeachTree::SetCircleEventValidity(BeachNode* arc_node, bool value) {
      if(!IsArc(arc_node))
        return;
      if(arc_node->value.arc->circle_event != nullptr)  
        arc_node->value.arc->circle_event->valid = value;  
    }

    void BeachTree::SetArcNeighbours(BeachNode* arc_node, Breakpoint* bp_left, Breakpoint* bp_right) {
      SPG_ASSERT(IsArc(arc_node))
      Arc* arc = GetArc(arc_node);
      arc->left_bp = bp_left;
      arc->right_bp = bp_right;
    }

    void BeachTree::SetBreakpointNeighbours(BeachNode* bp_node, Arc* arc_left, Arc* arc_right) {
      SPG_ASSERT(IsBreakpoint(bp_node))
      Breakpoint* bp = GetBreakpoint(bp_node);
      bp->left_arc = arc_left;
      bp->right_arc = arc_right;
    }

    void BeachTree::SetArcNeighbours(BeachNode* arc_node, BeachNode* bp_node_left, BeachNode* bp_node_right) {
      SPG_ASSERT(IsArc(arc_node) && IsBreakpoint(bp_node_left) && IsBreakpoint(bp_node_right))
      Arc* arc = GetArc(arc_node);
      arc->left_bp = GetBreakpoint(bp_node_left);
      arc->right_bp = GetBreakpoint(bp_node_right);
    }

    void BeachTree::SetBreakpointNeighbours(BeachNode* bp_node, BeachNode* arc_node_left, BeachNode* arc_node_right) {
      SPG_ASSERT(IsBreakpoint(bp_node) && IsArc(arc_node_left) && IsArc(arc_node_right))
      Breakpoint* bp = GetBreakpoint(bp_node);
      bp->left_arc = GetArc(arc_node_left);
      bp->right_arc = GetArc(arc_node_right);
    }

    Arc* BeachTree::LeftArc(BeachNode* node) {
      if(node == nullptr)
        return nullptr;
      if(IsBreakpoint(node)) 
        return node->value.breakpoint->left_arc;
      else { //node is an arc
        Breakpoint* left_bp = node->value.arc->left_bp;
        return left_bp == nullptr? nullptr : left_bp->left_arc;
      }
    }

    Arc* BeachTree::RightArc(BeachNode* node) {
      if(node == nullptr)
        return nullptr;
      if(IsBreakpoint(node)) 
        return node->value.breakpoint->right_arc;
      else { //node is an arc
        Breakpoint* right_bp = node->value.arc->right_bp;
        return right_bp == nullptr? nullptr : right_bp->right_arc;
      }
    }

    Breakpoint* BeachTree::LeftBreakpoint(BeachNode* node) {
      if(node == nullptr)
        return nullptr;
      if(IsArc(node)) 
        return node->value.arc->left_bp;
      else { //node is bp
        Arc* left_arc = node->value.breakpoint->left_arc;
        return left_arc == nullptr? nullptr : left_arc->left_bp;
      }
    }

    Breakpoint* BeachTree::RightBreakpoint(BeachNode* node) {
      if(node == nullptr)
        return nullptr;
      if(IsArc(node)) 
        return node->value.arc->right_bp;
      else { //node is bp
        Arc* right_arc = node->value.breakpoint->right_arc;
        return right_arc == nullptr? nullptr : right_arc->right_bp;
      }
    }

    //Arc* BeachTree::NthLeftArc(BeachNode* node, uint32_t n) {}
    //Arc* BeachTree::NthRightArc(BeachNode* node, uint32_t n) {}
    //Breakpoint* BeachTree::NthLeftBreakpoint(BeachNode* node, uint32_t n) {}
    //Breakpoint* BeachTree::NthRightBreakpoint(BeachNode* node, uint32_t n) {}

    std::string BeachElement::ToString(BeachElement const & el) {
      SPG_ASSERT(el.ctx != nullptr);
      auto& beach = el.ctx->GetBeachTree();
      float sweep_y = el.ctx->GetSweepY();
      std::string s{""};
      if(el.is_arc) {
        Arc* arc = el.arc;
        s = std::format("ARC:{}, S:({},{}) ",arc->id, arc->site->x, arc->site->y);
        if(arc->left_bp == nullptr)
          s += std::format(", BP_l:Nil");
        else {
          s += std::format(", BP_l:{} ", arc->left_bp->id);
        }
        if(arc->right_bp == nullptr) 
          s += std::format(", BP_r:Nil");
        else {
          float arc_x = arc->right_bp->CurrentX(sweep_y);
          s += std::format(", BP_r:{}, X:{} ",arc->right_bp->id ,arc_x);
        }
      }
      else {
        Breakpoint* bp = el.breakpoint;
        float bp_x = bp->CurrentX(sweep_y);
        s = std::format("BP:{}, AL:{}, AR:{}, X:{}",bp->id, bp->left_arc->id, bp->right_arc->id, bp_x);
      }
      return s;
    }

    void Voronoi::Test() 
    {
      SPG_WARN("-------------------------------------------------------------------------");
      SPG_WARN("Voronoi V3 - Test");
      SPG_WARN("-------------------------------------------------------------------------");

      Voronoi voronoi;
      //Event queue
      {
        SPG_WARN("EVENT QUEUE");
        const uint32_t NUM_VALS = 40;
        const float MIN_VAL = 0;
        const float MAX_VAL = 100;
        
        std::random_device rd;                         
        std::mt19937 mt(rd()); 
        std::uniform_real_distribution<float> fdist(MIN_VAL, MAX_VAL); 
        
        //add some site events
        for(int i=0; i< 10; i++) {
          Point2d* p = new Point2d(fdist(mt),fdist(mt));
          Event* e = new Event;
          e->type = Event::Type::Site;
          e->point = p;
          voronoi.m_event_queue.Push(e);
        }

        //add some circle events
        for(int i=0; i< 10; i++) {
          Point2d* p = new Point2d(fdist(mt),fdist(mt));
          Event* e = new Event;
          e->type = Event::Type::Circle;
          e->point = p;
          voronoi.m_event_queue.Push(e);
        }

        //print
        SPG_INFO("Event Queue:")
        while(!voronoi.m_event_queue.IsEmpty()) {
          Event* e = voronoi.m_event_queue.Pop();
          if(e->type == Event::Type::Site) {
            SPG_TRACE("Site: {}", *e->point);
          }
          else {
            SPG_TRACE("Circle: {}", *e->point);
          }
        }

      }

      //Parabola intersections
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

      //Bisectors and intersections
      {
        SPG_WARN("BISECTORS");
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

      //Beach line
      {
        SPG_WARN("BEACH LINE");
        
        std::vector<Point2d> points{{50,10},{54,9},{48,7},{47.3,5.5}};
        Voronoi voronoi(std::move(points));
        voronoi.Construct();
        // while(!voronoi.m_event_queue.IsEmpty()) {
        //   Event* e = voronoi.m_event_queue.Pop();
        //   voronoi.HandleSiteEvent(e);
        //   voronoi.PrintBeach();
        // }
        voronoi.m_beach.Validate();
      }

    }
    
  } //namespace Voronoi_V3

} //namespace Geom