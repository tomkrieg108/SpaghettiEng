#include "Voronoi.h"

#include <iostream>
#include <optional>
#include <format>
#include <locale>
#include <unordered_set>


namespace Geom
{
  static std::optional<std::pair<double,double>> 
  ComputeParabolaZeros(double a, double b, double c) noexcept
  {
    // roots of ax^2 + bx + c
    if(Equal(a, 0)) {
      SPG_ASSERT(!Equal(b,0.0));
      auto x = -c/b;
      return std::optional(std::pair(x,x));
    }
    auto discr = b*b - 4*a*c;
    if(discr < 0) {
      SPG_ERROR("No zeros. a,c,c, Discr: {} {} () {}", a,b,c, discr);
      SPG_ASSERT(false);
      return std::nullopt;
    }
      
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
    if(p1.IsDegenerate() && p2.IsDegenerate())
      return std::nullopt; // both vertical lines - no intersection
    if(p1.IsDegenerate()) { // p1 is a vertical line
      Point2d pt(float(p1.c), p2.GetY(p1.c));
      return std::optional(std::pair(pt,pt));
    }
    if(p2.IsDegenerate()) { //p2 is a vertical line
      Point2d pt(float(p2.c), p1.GetY(p2.c));
      return std::optional(std::pair(pt,pt));
    }

    auto a = p1.a-p2.a;
    auto b = p1.b-p2.b;
    auto c = p1.c-p2.c;
    auto result = ComputeParabolaZeros(a,b,c);
    if(!result.has_value()) {
      SPG_ASSERT(false);
      return std::nullopt;
    }
      
    auto& [x1,x2] = result.value();
    auto y1 = p1.GetY(x1);
    auto y2 = p1.GetY(x2);

    auto y1_ = p2.GetY(x1);
    auto y2_ = p2.GetY(x2);
    if(!Equal(y1, y1_)) {
      SPG_ERROR("y1: {}, p2.GetY(x1) {}",y1, y1_)
      SPG_ASSERT(false); 
    }
    if(!Equal(y2, y2_)) {
      SPG_ERROR("y1: {}, p2.GetY(x1) {}",y1, y1_)
      SPG_ASSERT(false); 
    }
    //note: x1,x2 (returned by ComputeParabolaZeros()) are doubles - need to cast to floats
    auto points = std::pair(Point2d((float)x1,y1), Point2d((float)x2,y2));
    return std::optional(points); 
  }

  namespace Voronoi_V4
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

    static float SignedArea(const Point2d& a, const Point2d& b, const Point2d& c)
    {
      //returns Det(a->b, a->c)*0.5. 
      double ax = a.x, ay = a.y;
      double bx = b.x, by = b.y;
      double cx = c.x, cy = c.y;
      double signed_area = 0.5 * ((bx - ax) * (cy - ay) - (cx - ax) * (by - ay));
      return static_cast<float>(signed_area);
    }

    Point2d Voronoi::ComputeBreakpointCoords(Breakpoint* bp) {
      SPG_ASSERT(bp != nullptr);
      SPG_ASSERT(bp->left_arc != nullptr);
      SPG_ASSERT(bp->right_arc != nullptr);
      
      Point2d left_site = *(bp->left_arc->site);
      Point2d right_site = *(bp->right_arc->site);
     
      Parabola left_parab(left_site, m_sweep);
      Parabola right_parab(right_site, m_sweep);

      auto result = ComputeIntersections(left_parab,right_parab);
      SPG_ASSERT(result.has_value());
      auto& [point1,point2] = result.value();
      if(Equal(point1,point2))
        return point1;   //1 intersection only => y-coords of sites are equal.

      if(left_site.y > right_site.y) 
        return point1.x < point2.x ? point1 : point2;
      else
        return  point1.x > point2.x ? point1 : point2;
    }

    float Breakpoint::CurrentX(float sweep_y)  {
      Voronoi* ctx = tree_node->value.ctx;
      float x_val = ctx->ComputeBreakpointCoords(this).x;
      return x_val;
    }

    Point2d Breakpoint::CurrentPos(float sweep_y)  {
      Voronoi* ctx = tree_node->value.ctx;
      return ctx->ComputeBreakpointCoords(this);
    }

    Voronoi::Voronoi(std::vector<Point2d> points) : m_points{std::move(points)} {
      m_event_queue.Initialize(m_points);
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
      m_sweep = e->point->y;

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
      auto arc_node_new = replacement_node_list[2];
      auto arc_triple_left = m_beach.GetArcTriple(m_beach.LeftArc(arc_node_new));
      auto arc_triple_right = m_beach.GetArcTriple(m_beach.RightArc(arc_node_new));
      TryInsertCircleEvent(arc_triple_right);
      TryInsertCircleEvent(arc_triple_left);

      SPG_WARN("FINISHED HANDLING SITE EVENT: {}", *e->point)
      //PrintBeach();
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
  
      // Create a new breakpoint (the merged left/right bp)
      CircleData circle = e->circle;
      auto* merged_bp_node = m_beach.MakeBreakpointNode();
      // set position rank of new breakpoint equal to that of the disappearing arc
      merged_bp_node->value.x_pos_rank = disappearing_arc_node->value.x_pos_rank;

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

      TryInsertCircleEvent(m_beach.GetArcTriple(prev_arc));
      TryInsertCircleEvent(m_beach.GetArcTriple(next_arc));

      SPG_WARN("FINISHED HANDLING CIRCLE EVENT: {}", *e->point)
      //PrintBeach();
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
     
      //Higher precision than above
      CircleData circle = CircumCircle(*arc_triple[0]->site, *arc_triple[1]->site, *arc_triple[2]->site);
      Point2d q = circle.center;
      float radius = circle.radius;

      //Validation!
      SPG_ASSERT(Equal(radius, glm::length(q-*(arc_triple[1]->site))));
      SPG_ASSERT(Equal(radius, glm::length(q-*(arc_triple[2]->site))));

      //float signed_area = ComputeSignedArea(*arc_triple[0]->site, *arc_triple[1]->site, *arc_triple[2]->site); //in Utils 
      float signed_area = SignedArea(*arc_triple[0]->site, *arc_triple[1]->site, *arc_triple[2]->site); // above - uses doubles
      // less than zero => CC orientation (required for convergent breakpoints)

      if(radius > 2000.0f) {
        SPG_WARN("CIRCLE EVENT NOT ADDED (radius too big) {}", radius);
        return;
      }
      if(std::fabs(signed_area) < 500.0*1e-6) {
        SPG_WARN("CIRCLE EVENT NOT ADDED (points nearly colinear) {}", signed_area);
        return;
      }

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

    /*
      Attach remaining edges on the beacline to the bounding box.
      This is not currently used (since it doesn't work!)
    */
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
        //Point2d cur = GetBreakpointCoords(bp);
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
        SPG_ASSERT(h_bp->origin != nullptr); // Todo Triggered occasionally
        SPG_ASSERT(h_bp->twin->origin == nullptr);
        Point2d origin = h_bp->origin->point;

        Point2d cur = bp->CurrentPos(m_sweep);
        //Point2d cur = bp->CurrentPos(m_sweep_prev + 20.0f); //Todo - this can cause a problem (adjusted sweep > site pos => no roots)
        //Point2d cur = GetBreakpointCoords(bp);
        
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
      return el1.x_pos_rank < el2.x_pos_rank;
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

    BeachTree::BeachNode* BeachTree::FindArcNodeAbove(Point2d const * site, float sweep_y) {
      SPG_ASSERT(site != nullptr);

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

      //Create the 5 new nodes needed - These are of type BeachElement, whicj are nodes in type BeachTree
      node_list[0] = MakeArcNode(replaced_arc->site);
      node_list[1] = MakeBreakpointNode();
      node_list[2] = MakeArcNode(site_event->point);
      node_list[3] = MakeBreakpointNode();
      node_list[4] = MakeArcNode(replaced_arc->site);

      //Set x_pos_ranks for each node - used by BeachElementComp when placing these nodes in the BeachTree
      Breakpoint* left_bp = LeftBreakpoint(arc_node_above);
      Breakpoint* right_bp = RightBreakpoint(arc_node_above);
      auto x_rank_prev = std::numeric_limits<XRankType>::min();
      auto x_rank_next = std::numeric_limits<XRankType>::max();
      if(left_bp != nullptr) // This arc is at the far left of the beachline
        x_rank_prev = left_bp->tree_node->value.x_pos_rank;
      if(right_bp != nullptr) // This arc is at the far left of the beachline
        x_rank_next = right_bp->tree_node->value.x_pos_rank;
      XRankType interval = x_rank_next - x_rank_prev;
      SPG_ASSERT(interval >= 12)
      if(interval < 24) {
        SPG_WARN("Positional ranking interval: {}.  Resetting ",interval);
        interval = ResetPoitionalRankings();
      }
      auto step = interval/6;
      for(uint32_t i=0; i<5; i++)
        node_list[i]->value.x_pos_rank = x_rank_prev + (i+1)*step;

      // Set the neighbours for each of the new nodes
      SetArcNeighbours(node_list[0], LeftBreakpoint(arc_node_above), GetBreakpoint(node_list[1]));
      SetArcNeighbours(node_list[2], GetBreakpoint(node_list[1]), GetBreakpoint(node_list[3]));
      SetArcNeighbours(node_list[4], GetBreakpoint(node_list[3]), RightBreakpoint(arc_node_above));
      SetBreakpointNeighbours(node_list[1], GetArc(node_list[0]),GetArc(node_list[2]));
      SetBreakpointNeighbours(node_list[3], GetArc(node_list[2]),GetArc(node_list[4]));

      return node_list;
    }

    BeachTree::XRankType BeachTree::ResetPoitionalRankings() {
      auto num_elements = static_cast<XRankType>(Size());
      constexpr auto  x_rank_min = std::numeric_limits<XRankType>::min();
      constexpr auto x_rank_max = std::numeric_limits<XRankType>::max();
      XRankType interval = (x_rank_max - x_rank_min) / (num_elements+1);

      auto first = begin();
      auto* node = first.Get();
      XRankType rank = x_rank_min;
      while(true) {
        auto& element = node->value;
        rank += interval;
        element.x_pos_rank = rank;
        
        if(node->value.is_arc) {
          Breakpoint* bp_right = node->value.arc->right_bp;
          if(bp_right == nullptr)
            break;
          node = bp_right->tree_node;
        }
        else {
          Arc* arc_right = node->value.breakpoint->right_arc;
          if(arc_right == nullptr)
            break;
          node = arc_right->tree_node;
        }
      }
      return interval;
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
      }
      else {
        Breakpoint* bp = el.breakpoint;
        s = Breakpoint::ToString(bp,sweep_y);
      }
      s += std::format(std::locale("en_US.UTF-8"), ", X-Rank: {:L}", el.x_pos_rank);
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
      SPG_WARN("Voronoi V4 - Test");
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
    
  } //namespace Voronoi_V4


} //namespace Geom