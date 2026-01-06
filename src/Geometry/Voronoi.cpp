#include "Voronoi.h"
#include <iostream>
#include <optional>
#include <format>
#include <unordered_set>

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

  //return intersection points of 2 parabolas if exist
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

  static int max_countx=0;
  static int tot_countx=0;
  static float ave_countx=0;
  static bool IsValidBreakpoint(Parabola const& arc_left, Parabola const& arc_right, float break_x) 
  {
    //todo - check case of degenerate arc
    auto eps = 1e-4;
    int count = 0;  
    while(true) {
      if(++count > 10)
        break;
      auto arc_left_yl = arc_left.GetY(break_x - eps);
      auto arc_left_yr = arc_left.GetY(break_x + eps);

      auto arc_right_yl = arc_right.GetY(break_x - eps);
      auto arc_right_yr = arc_right.GetY(break_x + eps);

      if(Equal(arc_left_yl,arc_right_yl) || Equal(arc_left_yr,arc_right_yr))
        eps *=4;
      else 
        return (arc_left_yl < arc_right_yl) && ( arc_left_yr > arc_right_yr);
    }
    SPG_ERROR("eps: {}", eps);
    SPG_ASSERT(false); //TODO: Often triggered
    return false;
  }

  static Point2d ComputeBreakpoint(Point2d const& left_site, Point2d const& right_site, float sweep_y) 
  {
    Parabola left_parab(left_site, sweep_y);
    Parabola right_parab(right_site, sweep_y);
    
    if(left_parab.IsDegenerate())
      return left_site;
    if(right_parab.IsDegenerate())
      return right_site;

    auto result = ComputeIntersections(left_parab,right_parab);
    SPG_ASSERT(result.has_value());
    auto& [point1,point2] = result.value();
    if(Equal(point1,point2))
      return point1;   //1 intersection only => y-coords of sites are equal.
    
    bool pnt1_valid = IsValidBreakpoint(left_parab,right_parab,point1.x);
    bool pnt2_valid = IsValidBreakpoint(left_parab,right_parab,point2.x);

    if(pnt1_valid && !pnt2_valid)
      return point1;
    if(pnt2_valid && !pnt1_valid)  
      return point2; //Todo: Always this one! Investigate
    
    SPG_ASSERT(false);
    return point1;     
  }

  namespace Voronoi_V3
  {
    uint32_t BeachElement::next_id = 0;

    static CircleData CircumCircle(Point2d const& a, Point2d const& b, Point2d const& c) {

      CircleData out;
      out.center = {0,0};
      out.radius = -1.0; //Indicates invalid   

      double ax = a.x, ay = a.y;
      double bx = b.x, by = b.y;
      double cx = c.x, cy = c.y;

      double d = 2.0 * (
          ax * (by - cy) +
          bx * (cy - ay) +
          cx * (ay - by)
      );

      if (std::abs(d) < 1e-6)
          return out; // collinear or nearly so

      double ax2ay2 = ax*ax + ay*ay;
      double bx2by2 = bx*bx + by*by;
      double cx2cy2 = cx*cx + cy*cy;

      double ux = (
          ax2ay2 * (by - cy) +
          bx2by2 * (cy - ay) +
          cx2cy2 * (ay - by)
      ) / d;

      double uy = (
          ax2ay2 * (cx - bx) +
          bx2by2 * (ax - cx) +
          cx2cy2 * (bx - ax)
      ) / d;

      double dx = ux - ax;
      double dy = uy - ay;

      out.center = { static_cast<float>(ux), static_cast<float>(uy) };
      out.radius = static_cast<float>(std::sqrt(dx*dx + dy*dy));

      return out;
    }

    Voronoi::Voronoi(std::vector<Point2d> points) : m_points{std::move(points)} {
      m_event_queue.Initialize(m_points);
      //m_beach.SetComparator(BeachNodeComp(this));
      m_beach.ctx = this;
    }

    void Voronoi::Construct() {
      Event* last_event = nullptr;
      while(!m_event_queue.IsEmpty()) {
        Event* e = m_event_queue.Pop();
        if(e->type == Event::Type::Site )
          HandleSiteEvent(e);
        else 
          HandleCircleEvent(e);
        last_event = e;  
      }
      SPG_ASSERT(last_event->type == Event::Type::Circle)
      //TieLooseEnds();
    }

    void Voronoi::HandleSiteEvent(Event* e) {
      SPG_WARN("HANDLING SITE EVENT: {}", *e->point)
      m_sweep_prev = m_sweep;
      m_sweep = e->point->y - 0.001f; //Prevent degenerate arcs 

      //* STEP 1.
      if(m_beach.Empty()) {
        auto* arc_node = m_beach.MakeArcNode(e->point);
        m_beach.Insert(arc_node);
        return;
      }
      
      //* STEP 2
      auto* arc_node_above = m_beach.FindArcNodeAbove(e->point, m_sweep);
      // If arc_node_above has a circle event then invalidate it:
      Arc* arc_above = m_beach.GetArc(arc_node_above);
      if(arc_above->circle_event != nullptr) {
         arc_above->circle_event->valid = false;
         SPG_TRACE("Invalidating Circle event at: {}, arc: {}, m_sweep:{}", *arc_above->circle_event->point, Arc::ToString(arc_above, m_sweep),m_sweep);
      }
       
      //* STEP 3
      auto replacement_node_list = m_beach.MakeNodeList(e, arc_node_above);
      m_beach.InsertNodeList(e, replacement_node_list, arc_node_above); // This also erases arc_node_above from tree
      
      //* STEP 4 - setup dcel half edges
      auto half_edge_pair = m_dcel.MakeHalfEdgePair();
      auto left_bp = m_beach.GetBreakpoint(replacement_node_list[1]);
      auto right_bp = m_beach.GetBreakpoint(replacement_node_list[3]);
      left_bp->half_edge = half_edge_pair.first;
      right_bp->half_edge = half_edge_pair.second;

      //* STEP 5
    #if 0
      Arc* arc_new = m_beach.GetArc(replacement_node_list[2]);
      BreakpointPair bp_left_pair = m_beach.GetLeftBreakpointPair(arc_new);
      BreakpointPair bp_right_pair = m_beach.GetRightBreakpointPair(arc_new);
      InsertCircleEventIfNeeded(bp_left_pair,arc_new); //second param only needed for debug/validation
      InsertCircleEventIfNeeded(bp_right_pair,arc_new); //second param only needed for debug/validation
    #endif

      auto arc_node_new = replacement_node_list[2];
      auto arc_triple_left = m_beach.GetArcTriple(m_beach.LeftArc(arc_node_new));
      auto arc_triple_right = m_beach.GetArcTriple(m_beach.RightArc(arc_node_new));
      TryInsertCircleEvent(arc_triple_right);
      TryInsertCircleEvent(arc_triple_left);

      SPG_WARN("FINISHED HANDLING SITE EVENT: {}", *e->point)
      PrintBeach();
    }

    void Voronoi::HandleCircleEvent(Event* e) {
      SPG_ASSERT(e != nullptr);
      SPG_WARN("HANDLING CIRCLE EVENT (m_sweep): {}", *e->point)
      // Update sweepline
      m_sweep_prev = m_sweep;
      m_sweep = e->point->y;

      SPG_TRACE("Disappearing Arc: {}", e->diappearing_arc->id);
      if(!e->valid)  {
        SPG_TRACE("Has been invalidated");
        return;
      }

  #if 0
      SPG_INFO("######################################################################");
      SPG_INFO("Beachline just before merge (sweep nudged up slightly):");
      m_sweep += 0.001f;
      PrintBeach();
      m_sweep = e->point->y;
      SPG_INFO("-----------------------------------------------------------------------");
  #endif  

      SPG_ASSERT(e->diappearing_arc != nullptr);
      auto* disappearing_arc_node = e->diappearing_arc->tree_node;
      
      //* STEP 1 - erase the disappearing arc, and its neighbouring breakpoints.   Merge into a new breakpoint
      Breakpoint* left_bp = m_beach.LeftBreakpoint(disappearing_arc_node);
      Breakpoint* right_bp = m_beach.RightBreakpoint(disappearing_arc_node);
      
      //Invalidate any other circle events associated with the neighbours of the disappearing arc
      Arc* prev_arc = m_beach.LeftArc(disappearing_arc_node);
      Arc* next_arc = m_beach.RightArc(disappearing_arc_node);
      SPG_ASSERT(prev_arc != nullptr && next_arc != nullptr);
      if(prev_arc->circle_event != nullptr) {
        prev_arc->circle_event->valid = false;
         SPG_TRACE("Invalidating Circle event at: {}, arc {}, m_sweep:{}", *prev_arc->circle_event->point, Arc::ToString(prev_arc, m_sweep), m_sweep);
      }
      if(next_arc->circle_event != nullptr) {
        next_arc->circle_event->valid = false;
        SPG_TRACE("Invalidating Circle event at: {}, arc {}, m_sweep:{}", *next_arc->circle_event->point, Arc::ToString(next_arc, m_sweep),m_sweep);
      }
  
  // This next stuff here is wrong!
  #if 0    
      while(prev_arc != nullptr) {
        if( (prev_arc->site == e->diappearing_arc->site) && (prev_arc->circle_event != nullptr) ) 
          prev_arc->circle_event->valid = false;
        prev_arc = m_beach.LeftArc(prev_arc->tree_node);
      }
      while(next_arc != nullptr) {
        if( (next_arc->site == e->diappearing_arc->site) && (next_arc->circle_event != nullptr) ) 
          next_arc->circle_event->valid = false;
        next_arc = m_beach.RightArc(next_arc->tree_node);
      }
  #endif
      
  // Validation...
  #if 0
      float x_left = left_bp->CurrentX(m_sweep);
      float x_right = right_bp->CurrentX(m_sweep);
      SPG_ASSERT(Equal(x_left,x_right));
      SPG_ASSERT(Equal(x_left, e->circle.center.x));
      SPG_ASSERT(Equal(e->circle.center.y - e->circle.radius, m_sweep));
  #endif

      // Create a new breakpoint (the merged left/right bp)
      CircleData circle = e->circle;
      auto* merged_bp_node = m_beach.MakeBreakpointNode();
      m_beach.SetBreakpointNeighbours(merged_bp_node, prev_arc, next_arc); 
      prev_arc->right_bp = next_arc->left_bp = merged_bp_node->value.breakpoint;  
      
      //Note: These records get erased from the beach tree, but are retained in this object (m_arcs, m_breakpoints)
      m_beach.Erase(disappearing_arc_node);
      m_beach.Erase(left_bp->tree_node);
      m_beach.Erase(right_bp->tree_node);
      m_beach.Insert(merged_bp_node);

      //* STEP 2: Add the center of the circle causing the event as a vertex record in the DCEL, create Half edge records for the new (merged) breakpoint.
      m_bounding_box.Update(circle.center);
      auto half_edge_pair = m_dcel.MakeHalfEdgePair();
      Breakpoint* merged_bp = m_beach.GetBreakpoint(merged_bp_node);
      merged_bp->half_edge = half_edge_pair.first;
      m_dcel.Connect(circle.center, {right_bp->half_edge, left_bp->half_edge, merged_bp->half_edge});

      //* STEP 3: Check the new triple of consecutive arcs that has the former left neighbour of the disappearing arc as the middle of the triple.  If so add circle event.  Repeat for the where the former right neighbour is the middle arc

  #if 0
      BreakpointPair bp_left_pair = m_beach.GetBreakpointPair(prev_arc);
      BreakpointPair bp_right_pair = m_beach.GetBreakpointPair(next_arc);
      InsertCircleEventIfNeeded(bp_left_pair, prev_arc);
      InsertCircleEventIfNeeded(bp_right_pair, next_arc);
  #endif

      TryInsertCircleEvent(m_beach.GetArcTriple(prev_arc));
      TryInsertCircleEvent(m_beach.GetArcTriple(next_arc));

      SPG_WARN("FINISHED HANDLING CIRCLE EVENT: {}", *e->point)
      PrintBeach();
    }

    void Voronoi::TryInsertCircleEvent(BeachTree::ArcTriple const& arc_triple) {
      SPG_INFO("TRY ADDING CIRCLE EVENT:")
      SPG_ASSERT(arc_triple[1] != nullptr);  //middle arc
      if(arc_triple[0] == nullptr || arc_triple[2] == nullptr) { //Left,right arcs
        SPG_TRACE("No arc triplet for middle arc: {}", arc_triple[1]->tree_node->value);
        return;
      }
      SPG_TRACE("Arc Disappearing:  {}", arc_triple[1]->tree_node->value);
      SPG_TRACE("BP left:  {}", Breakpoint::ToString(arc_triple[1]->left_bp, m_sweep));
      SPG_TRACE("BP right:  {}", Breakpoint::ToString(arc_triple[1]->right_bp, m_sweep));

      //Todo: ComputeIntersection() is all done with floats, not doubles => larger errors introduced.  Need computations using doubles ideally.  In the meantime, need to allow larger tolerance for Eqaul()
      Line2d bisector_left = GetBisector(*arc_triple[0]->site, *arc_triple[1]->site);
      Line2d bisector_right = GetBisector(*arc_triple[1]->site, *arc_triple[2]->site);
      // circumcenter of the 3 points
      Point2d q = ComputeIntersection(bisector_left,bisector_right); 
      float radius = glm::length(q-*(arc_triple[0]->site));
      //Validation!
      SPG_ASSERT(Equal(radius, glm::length(q-*(arc_triple[1]->site))));
      SPG_ASSERT(Equal(radius, glm::length(q-*(arc_triple[2]->site))));

      CircleData circle = CircumCircle(*arc_triple[0]->site, *arc_triple[1]->site, *arc_triple[2]->site);
      float signed_area = ComputeSignedArea(*arc_triple[0]->site, *arc_triple[1]->site, *arc_triple[2]->site);
      // less than zero => CC orientation (required for convergent breakpoints)

      Arc* disappearing_arc = arc_triple[1];
      float circle_bottom = q.y - radius;

      bool breakpoints_diverging = (signed_area > 0) || (circle_bottom > m_sweep) || Equal(circle_bottom,m_sweep) || (Equal(signed_area,0)); //area of zero means 3 points are colinear

      //could do a direct check also. Calculate dist between bp's, nudge sweep down, re-calculate. New dist greater or less?
    
      if(breakpoints_diverging) {
        SPG_INFO("CIRCLE EVENT NOT ADDED (breakpoints not converging)");
        return;
      }
      
      Event* circle_event = MakeCircleEvent(Point2d(q.x, circle_bottom),CircleData(q,radius), disappearing_arc);
      m_event_queue.Push(circle_event);
      SPG_INFO("ADDED CIRCLE EVENT: Arc Disappearing: {}", Arc::ToString(disappearing_arc, m_sweep));
    }

    void Voronoi::TieLooseEnds() {
      //* Add bounding box to m_dcel
      m_bounding_box.AddBorder(20.0f);
      std::vector<Point2d> bb_points = m_bounding_box.GetPoints();
      SPG_WARN("TIE LOOSE ENDS");
      SPG_INFO("BOUNDING BOX POINTS");
      for(auto& p : bb_points) {
        SPG_TRACE("{}",p);
      }
      auto bb_half_edges = m_dcel.InsertBoundingBox(m_bounding_box);

      //* for each BP remaining in the tree, get it's half edge, get its origin, get its direction based on BP position at current sweep position.  Store them in a vector of lineSeg2D
      //std::vector<LineSeg2D> bp_segs; // Line Segs being traced out for each remaining breakpoint
      for(auto& element : m_beach) {
        if(element.is_arc)
          continue;
        Breakpoint* bp = element.breakpoint;
        SPG_ASSERT(bp != nullptr)
        DCEL::HalfEdge* h_bp = bp->half_edge;

        //set the face of the 'dangling' bp half edges to the outer face of the bb_half_edges
        h_bp->incident_face = bb_half_edges[0]->incident_face;
        h_bp->twin->incident_face = bb_half_edges[0]->incident_face;

        //Ensure  h is connected at exactly 1 end, get the connection (origin) point
        if(h_bp->origin == nullptr) 
          h_bp = h_bp->twin;
        SPG_ASSERT(h_bp->origin != nullptr);
        SPG_ASSERT(h_bp->twin->origin == nullptr);
        Point2d origin = h_bp->origin->point;

        //Todo:  Might want to use m_sweep_prev.  m_sweep could be very big here
        Point2d cur = bp->CurrentPos(m_sweep);
        // the half edge is connected to DCEL at origin, unconnected at cur
        // extend cur to a point beyond bounding box
        glm::vec2 dir = glm::normalize(cur - origin);
        float scale = std::max(m_bounding_box.Width(), m_bounding_box.Height())*4.0f;
        cur = origin + dir*scale;
        LineSeg2D bp_seg(origin, cur);

        // Find intersection of breakpoint seg (bp_seg) with the bounding box (bb_seg)
        Point2d intersection;
        bool found = false;
        for(DCEL::HalfEdge* h_bb : bb_half_edges) {
          LineSeg2D bb_seg = m_dcel.GetLineSeg2d(h_bb);
          if(Geom::StrictIntersectionExists(bp_seg,bb_seg)) {
            found = ComputeIntersection(bb_seg,bp_seg,intersection);
            if(found) {
              // split the HalfEdge h at the intersection
              DCEL::Vertex* v = m_dcel.Split(intersection, h_bb);
              //Connect the Halfedge associate with the BP to Vertex v
              m_dcel.Connect(v,h_bp);
              break;
            }
          }
        }
        SPG_ASSERT(found == true);


        //bp_segs.push_back(seg);
      }

  #if 0
      //* Compute and save intersections of linesegs with bounding box.  Connect BP half edge with BB half adge at intersection
      //TODO: handle edge case where intersection occurs exacly at a vertex of the bounding box
      std::vector<Point2d> bounding_points;
      uint32_t i = 0; // index into bp_segs
      for(auto& element : m_beach) {
        if(element.is_arc)
          continue;
        auto bb_segs = m_bounding_box.GetLineSegs();
        Point2d intersection; 
        bool found = false;
        for(auto& bb_seg : bb_segs) {
          if(StrictIntersectionExists(bb_seg, bp_segs[i])); {
            found = ComputeIntersection(bb_seg,bp_segs[i],intersection);
            if(found)
              break;
          }
        }

        SPG_ASSERT(found);
        bounding_points.push_back(intersection);
        ++i;
      }
  #endif  

      //Include points from bb_points
      // for(auto& p : bb_points)
      //   bounding_points.push_back(p);

      //Validation only
      // Point2d bb_centroid1 = ComputeCentroid(bb_points); 
      // float area1 = Geom::SignedArea(bb_points);  
      // Point2d bb_centroid2 = ComputeCentroid(bounding_points); 
      // float area2 = Geom::SignedArea(bounding_points);  

      //* for each BP, join it's origin and the corresponding DCEL vertex.

      //* Join the half edge origin with intersection vertex.


    }

    void Voronoi::InsertCircleEventIfNeeded(BreakpointPair bp_pair, Arc* new_arc) {
      if(bp_pair.left == nullptr || bp_pair.right == nullptr) {
        SPG_INFO("No arc triplet for {}", new_arc->tree_node->value)
        return;
      }
      SPG_ASSERT(bp_pair.left->right_arc == bp_pair.right->left_arc);
      SPG_INFO("Breakpoints adjacent to new arc:")
      SPG_TRACE("Arc New:  {}", new_arc->tree_node->value);
      SPG_TRACE("Arc Disappearing:  {}", bp_pair.left->right_arc->tree_node->value);
      SPG_TRACE("BP left:  {}", bp_pair.left->tree_node->value);
      SPG_TRACE("BP right:  {}", bp_pair.right->tree_node->value);

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
      Arc* disappearing_arc = bp_pair.left->right_arc;
      float circle_bottom = q.y - radius;
      if(m_sweep > circle_bottom) {
        SPG_INFO("Breakpoints converging - Adding circle event")
        // Disappearing arc is bp_pair.left->right_arc
        Event* circle_event = MakeCircleEvent(Point2d(q.x, circle_bottom),CircleData(q,radius), disappearing_arc);
        circle_event->diappearing_arc = disappearing_arc;
        disappearing_arc->circle_event = circle_event;
        circle_event->valid = true;
        m_event_queue.Push(circle_event);
      }
      else {
        SPG_INFO("Breakpoints NOT converging - NOT Adding circle event")
      }
    }

    Arc* Voronoi::MakeArc(Point2d const * site_point) {
      m_arcs.push_back(std::make_unique<Arc>());
      Arc* arc= m_arcs.back().get();
      arc->site = site_point;
      arc->id = BeachElement::next_id++;
      return arc;
    }
    
    Breakpoint* Voronoi::MakeBreakpoint() {
      m_breakpoints.push_back(std::make_unique<Breakpoint>());
      Breakpoint* bp= m_breakpoints.back().get();
      bp->id = BeachElement::next_id++;
      return bp;
    }

    Event* Voronoi::MakeCircleEvent(Point2d const& point, CircleData const& circle, Arc* disappearing_arc) {
      m_circle_event_points.push_back(std::make_unique<Point2d>(point));
      m_circle_events.push_back(std::make_unique<Event>());
      Event* event = m_circle_events.back().get();
      event->point =  m_circle_event_points.back().get();
      event->type = Event::Type::Circle;
      event->diappearing_arc = disappearing_arc;
      event->circle = circle;
      event->valid = true;
      disappearing_arc->circle_event = event;
      return event;
    }

    void Voronoi::PrintBeach() {
  #if 1   
      SPG_WARN("Beach: (Via Tree Traversal), Y-SWEEP: {}", m_sweep)
      for(auto& element : m_beach) {
        SPG_TRACE("{}",element);
      }
  #endif

  // Arcs noc necessarily leaf nodes, BP's not necessarily internal nodes - not a valid check!
  #if 0
       for(auto itr = m_beach.begin(); itr != m_beach.end(); ++itr) {
        auto* node = itr.Get();
        if(m_beach.IsArc(node)) {
          if(!m_beach.IsLeaf(node)) {
            SPG_ERROR("Arc: {} Is not a leaf ", node->value);
          }
        }
        if(m_beach.IsBreakpoint(node)) {
          if(m_beach.NumChildren(node) != 2) {
            SPG_ERROR("Breakpoint: {} has {} children ", node->value, m_beach.NumChildren(node));
          }
        }
       }
  #endif   

  #if 1
      SPG_WARN("Beach: (Via Following internal links), Y-SWEEP: {}", m_sweep)
      auto first = m_beach.begin();
      auto* node = first.Get();
      SPG_TRACE("{}", node->value);
      while(true) {
        if(node->value.is_arc) {
          Breakpoint* bp_right = node->value.arc->right_bp;
          if(bp_right == nullptr)
            break;
          node = bp_right->tree_node;
          SPG_TRACE("{}",node->value);
        }
        else {
        Arc* arc_right = node->value.breakpoint->right_arc;
        if(arc_right == nullptr)
          break;
        node = arc_right->tree_node;
          SPG_TRACE("{}",node->value);  
        }
      }
  #endif
  }

    std::vector<Point2d> Voronoi::GetConnectedEdgePoints() {
      m_bounding_box.AddBorder(20.0f);
      std::vector<Point2d> points;
      std::unordered_set<DCEL::HalfEdge*> half_edges_processed;
      auto half_edges = m_dcel.GetHalfEdges();
      for(auto h : half_edges) {
        //if(half_edges_processed.contains(h))
        //  continue;
        SPG_ASSERT(h != nullptr)
        if(h->origin == nullptr)
          continue;
        if(h->next != nullptr && h->next->origin != nullptr) {
          points.push_back(h->origin->point);
          points.push_back(h->next->origin->point);
        }
        //half_edges_processed.insert(h);
        //half_edges_processed.insert(h->twin);
      }
      return points;
    }

    std::vector<Point2d> Voronoi::GetLooseEdgePoints() {
      std::vector<Point2d> points;
      for(auto& element : m_beach) {
        if(element.is_arc)
          continue;
        Breakpoint* bp = element.breakpoint;
        SPG_ASSERT(bp != nullptr)
        DCEL::HalfEdge* h_bp = bp->half_edge;

        //Ensure h_bp is connected at exactly 1 end, get the connection (origin) point
        if(h_bp->origin == nullptr) 
          h_bp = h_bp->twin;
        SPG_ASSERT(h_bp->origin != nullptr);
        SPG_ASSERT(h_bp->twin->origin == nullptr);
        Point2d origin = h_bp->origin->point;

        //Point2d cur = bp->CurrentPos(m_sweep);
        Point2d cur = bp->CurrentPos(m_sweep_prev + 20.0f);
        // the half edge is connected to DCEL at origin, unconnected at cur
        // extend cur to a point beyond bounding box
        glm::vec2 dir = glm::normalize(cur - origin);
        float scale = std::max(m_bounding_box.Width(), m_bounding_box.Height());
        cur = origin + dir*scale;
        points.push_back(origin);
        points.push_back(cur);
      }
      return points;
    }

    std::vector<Point2d> Voronoi::GetVertexPoints() {
      std::vector<Point2d> points;
      auto& verticies = m_dcel.GetVertices();
      for(auto v : verticies)
        points.push_back(v->point);
      return points;  
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
        SPG_ASSERT(false); //Should never happen since sweep line nudged down slightly to prevent this/
        //Todo:  triggered sometimes!
        return arc1.arc->id < arc2.arc->id; // Occurs for new site events if sweep line not moved down
      }
      return x1 < x2;  
    }

    bool BeachElementComp::CompArcToBP(BeachElement const & arc,  BeachElement const& bp) const {
      // return true if arc is to the left of bp
      if(arc.arc->left_bp == nullptr)
        return true; //arc is at extreme left of beachline
      if(arc.arc->right_bp == nullptr)
        return false; //arc is at extreme right of beachline
      float sweep_y = ctx->GetSweepY();
      Breakpoint* bp_right = arc.arc->right_bp;
      float arc_x = bp_right->CurrentX(sweep_y);
      float bp_x = bp.breakpoint->CurrentX(sweep_y);
      if(Equal(arc_x,bp_x))
        return true; //arcs right bp is this bp => arc is to the left
      return arc_x < bp_x;
    }

    bool BeachElementComp::CompBPToBP(BeachElement const & bp1,  BeachElement const& bp2) const {
      // return true if bp1 is to the left of bp2
      float x1 = bp1.breakpoint->CurrentX(ctx->GetSweepY());
      float x2 = bp2.breakpoint->CurrentX(ctx->GetSweepY());
      if(Equal(x1,x2)) {
        //TODO:  Triggered sometimes
        SPG_ASSERT(false); //Should never happen since sweep line nudged down slightly to prevent this
        return bp1.breakpoint->id < bp2.breakpoint->id; // Occurs for new site events if sweep line not moved down
      }
      return x1 < x2;  
    }

    BeachTree::BeachNode* BeachTree::MakeArcNode(Point2d const * site) {
      SPG_ASSERT(site != nullptr);
      BeachElement el;
      el.arc = ctx->MakeArc(site);
      el.is_arc = true;
      el.ctx = this->ctx; //only for printing
      BeachNode* node = this->MakeNode(el,m_nil);
      node->value.arc->tree_node = node;
      return node;
    }

    BeachTree::BeachNode* BeachTree::MakeBreakpointNode() {
      BeachElement el;
      el.breakpoint = ctx->MakeBreakpoint();
      el.is_arc = false;
      el.ctx = this->ctx; //only for printing
      BeachNode* node = this->MakeNode(el,m_nil);
      node->value.breakpoint->tree_node = node;
      return node;
    } 

    BeachTree::BeachNode* BeachTree::MakeBreakpointNode(Arc* left_arc, Arc* right_arc) {
      SPG_ASSERT(left_arc != nullptr && right_arc != nullptr);    
      BeachElement el;
      el.breakpoint = el.breakpoint = ctx->MakeBreakpoint();
      el.breakpoint->left_arc = left_arc;
      el.breakpoint->right_arc = right_arc;
      el.is_arc = false;
      el.ctx = this->ctx; //only for printing
      BeachNode* node = this->MakeNode(el,m_nil);
      node->value.breakpoint->tree_node = node;
      return node;
    }

    BeachTree::BeachNode* BeachTree::FindArcNodeAbove(Point2d const * site, float sweep_y) {
      SPG_ASSERT(site != nullptr);

      //This assumes that can only be leaves, breakpoints internal node - not the case!
  #if 0
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
  #endif

      auto node = m_root;
      while(node != m_nil) {
        float node_x = 0;
        if(IsBreakpoint(node)) {
          Breakpoint* bp = GetBreakpoint(node);
          node_x = bp->CurrentX(sweep_y);
          SPG_ASSERT(!Geom::Equal(node_x, site->x)); //Todo Edge case - need to handle
        } 
        else {
          Arc* arc = GetArc(node);
          float x_left = std::numeric_limits<float>::lowest();
          float x_right = std::numeric_limits<float>::max();
          if(arc->left_bp != nullptr) {
            Breakpoint* bp_l = GetBreakpoint(arc->left_bp->tree_node);
            x_left = bp_l->CurrentX(sweep_y);
          }
          if(arc->right_bp != nullptr) {
            Breakpoint* bp_r = GetBreakpoint(arc->right_bp->tree_node);
            x_right = bp_r->CurrentX(sweep_y);
          }
          if(site->x > x_left && site->x < x_right) {
            return node;
          }
          node_x = x_right;
        }
        if(site->x < node_x)
          node = node->left;
        else
          node = node->right;
      }
      SPG_ERROR("Arc above not found");
      SPG_ASSERT(false);
      return nullptr;
    }

    BeachTree::NodeList BeachTree::MakeNodeList(Event* site_event, BeachNode* arc_node_above) {
      NodeList node_list; 
      SPG_ASSERT(arc_node_above != nullptr);
      Arc* replaced_arc = GetArc(arc_node_above);
      SPG_ASSERT(replaced_arc != nullptr);
      node_list[0] = MakeArcNode(replaced_arc->site);
      node_list[1] = MakeBreakpointNode();
      node_list[2] = MakeArcNode(site_event->point);
      node_list[3] = MakeBreakpointNode();
      node_list[4] = MakeArcNode(replaced_arc->site);
      // node_list[1] = MakeBreakpointNode(GetArc(node_list[0]),GetArc(node_list[2]));
      // node_list[3] = MakeBreakpointNode(GetArc(node_list[2]),GetArc(node_list[4]));
      SetArcNeighbours(node_list[0], LeftBreakpoint(arc_node_above), GetBreakpoint(node_list[1]));
      SetArcNeighbours(node_list[2], GetBreakpoint(node_list[1]), GetBreakpoint(node_list[3]));
      SetArcNeighbours(node_list[4], GetBreakpoint(node_list[3]), RightBreakpoint(arc_node_above));
      SetBreakpointNeighbours(node_list[1], GetArc(node_list[0]),GetArc(node_list[2]));
      SetBreakpointNeighbours(node_list[3], GetArc(node_list[2]),GetArc(node_list[4]));

      //Todo - precalculate x-vals for each new element so they they don't need to be recalculated multiple times when inserted
       //chk only
      float sweep_y = ctx->GetSweepY();
      float x_bp1 = node_list[1]->value.breakpoint->CurrentX(sweep_y);
      float x_bp2 = node_list[3]->value.breakpoint->CurrentX(sweep_y);

      return node_list;
    }

    void BeachTree::InsertNodeList(Event* site_event, NodeList& node_list, BeachNode* arc_node_above) {
      // Check if arc_replaced has left/right BP's.  If so, the left/right nodes for these breakpoints need to be updated.
      Breakpoint* far_left_bp = LeftBreakpoint(arc_node_above);
      Breakpoint* far_right_bp = RightBreakpoint(arc_node_above);
      if(far_left_bp != nullptr)
        far_left_bp->right_arc = GetArc(node_list[0]);
      if(far_right_bp != nullptr)
        far_right_bp->left_arc = GetArc(node_list[4]);  
    
      this->Erase(arc_node_above);
      for(auto* node : node_list)
        this->Insert(node);
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

    BreakpointPair BeachTree::GetBreakpointPair(Arc* arc) {
      SPG_ASSERT(arc != nullptr)
      BreakpointPair bp_pair;
      bp_pair.left = arc->left_bp;
      bp_pair.right = arc->right_bp;
      return bp_pair;
    }

    BeachTree::ArcTriple BeachTree::GetArcTriple(Arc* middle_Arc) {
      ArcTriple arc_triple;
      arc_triple[0] = LeftArc(middle_Arc->tree_node);
      arc_triple[1] = middle_Arc;
      arc_triple[2] = RightArc(middle_Arc->tree_node);
      return arc_triple;
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

    std::string BeachElement::ToString(BeachElement const & el) {
      SPG_ASSERT(el.ctx != nullptr);
      auto& beach = el.ctx->GetBeachTree();
      float sweep_y = el.ctx->GetSweepY();
      std::string s{""};
      if(el.is_arc) {
        Arc* arc = el.arc;
        s = Arc::ToString(arc,sweep_y);
  #if 0  
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
  #endif
      }
      else {
        Breakpoint* bp = el.breakpoint;
        s = Breakpoint::ToString(bp,sweep_y);
    #if 0  
        float bp_x = bp->CurrentX(sweep_y);
        s = std::format("BP:{}, AL:{}({},{}), AR:{}({},{}), X:{}",
          bp->id, 
          bp->left_arc->id, bp->left_arc->site->x,bp->left_arc->site->y, 
          bp->right_arc->id,bp->right_arc->site->x,bp->right_arc->site->y,
          bp_x);
    #endif
      }
      return s;
    }

    std::string Breakpoint::ToString(Breakpoint* bp, float sweep_y) {
      std::string s{""};
      float bp_x = bp->CurrentX(sweep_y);
      s = std::format("BP:{}, AL:{}({},{}), AR:{}({},{}), X:{}",
      bp->id, 
      bp->left_arc->id, bp->left_arc->site->x,bp->left_arc->site->y, 
      bp->right_arc->id,bp->right_arc->site->x,bp->right_arc->site->y,
      bp_x);

      DCEL::HalfEdge* h = bp->half_edge;
      if(h != nullptr) {
        s += std::format(", HE:{} ", h->tag);
        if(h->origin != nullptr)
          s+= std::format("Orig: ({},{}) ", h->origin->point.x, h->origin->point.y);
        else 
          s+= std::format("Orig: Null ");    
        SPG_ASSERT(h->twin != nullptr);
        s += std::format("HE_tw:{} ", h->twin->tag);
        if(h->twin->origin != nullptr) {
           s+= std::format("Tw Orig: ({},{}) ", h->twin->origin->point.x,h->twin->origin->point.y);
        } else {  
          s+= std::format("Tw Orig: Null");
        }

      }

      return s;
    }

    std::string Arc::ToString(Arc* arc,float sweep_y) {
      std::string s{""};
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
        
        //std::vector<Point2d> points{{50,10},{54,9},{48,7},{47.3,5.5}}; //ok
        //std::vector<Point2d> points{{50,10},{54,9},{48,7},{47.3,5.5}, {53,5}}; //ok
        //std::vector<Point2d> points{{50,10},{54,9},{48,7},{47.3,5.5}, {53,5}, {52,3}}; //ok
        //std::vector<Point2d> points{{50,10},{54,9},{48,7},{47.3,5.5},{53,5},{52,3},{58,-2}}; //ok
        //std::vector<Point2d> points{{50,10},{54,9},{48,7},{47.3,5.5},{53,5},{52,3},{58,-2},{56,-3.5}}; //ok
       
        std::vector<Point2d> points{{50,10},{54,9},{48,7},{47.3,5.5}, {53,5}, {52,3}, {58,-2}, {56,-3.5},{44,0.8},{50,-7}}; 

        Voronoi voronoi(std::move(points));
        voronoi.Construct();

        SPG_WARN("FINAL BEACH TREE:");
        voronoi.PrintBeach();

        SPG_WARN("FINAL BEACH TREE VALIDATION:");
        voronoi.m_beach.Validate();


        //auto end_points = voronoi.GetVoronoiEdgeEndPoints();
        
        // SPG_WARN("DCEL:");
        // voronoi.m_dcel.PrintVertices();
        // voronoi.m_dcel.PrintHalfEdges();
        // voronoi.m_dcel.PrintFaces();
      }

    }
    
  } //namespace Voronoi_V3

} //namespace Geom