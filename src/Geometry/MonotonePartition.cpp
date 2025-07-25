
#include "MonotonePartition.h"
#include "GeomUtils.h"

namespace Geom
{
  using VertexCategory = MonotonePartitionAlgo::VertexCategory;

  MonotonePartitionAlgo::MonotonePartitionAlgo() :
      m_T(EdgeComparator(m_cur_event_point))
    {
    }

  MonotonePartitionAlgo::MonotonePartitionAlgo(const std::vector<Point2d>& points) : 
    m_polygon(points),
    m_T(EdgeComparator(m_cur_event_point))
  {
    m_polygon.Validate();
    InitialiseEventQueue();
  }

  void MonotonePartitionAlgo::Clear()
    {
        m_polygon.Clear();
        m_event_queue.clear();
        m_event_queue_unsorted.clear();
        m_T.clear();
        m_monotone_diagonals.clear();
        m_triangulation_diagonals.clear();
        m_cur_event_point = {FLT_MAX,FLT_MAX};
    }

  void MonotonePartitionAlgo::Set(const std::vector<Point2d>& points)
  {
    Clear();
    m_polygon.Set(points);
    m_polygon.Validate();
    InitialiseEventQueue();
  }

  VertexCategory MonotonePartitionAlgo::GetVertexCategory(DCEL::Vertex* vertex)
  {
    auto e_departing_v = GetDepartingEdge(vertex);
    SPG_ASSERT(e_departing_v != nullptr);
    SPG_ASSERT(e_departing_v->origin = vertex);
    SPG_ASSERT(e_departing_v->next != nullptr);
    SPG_ASSERT(e_departing_v->prev != nullptr); 

    auto v_prev = e_departing_v->prev->origin;  
    auto v_next = e_departing_v->next->origin;
    SPG_ASSERT(v_prev != nullptr);
    SPG_ASSERT(v_next != nullptr);

    if( (v_prev->point < vertex->point) && (v_next->point < vertex->point)) {
      //both neighbours are below vertex => Start or Split
      Geom::LineSeg2D seg{v_prev->point, vertex->point};
      if(Geom::LeftOrBeyond(seg,v_next->point)) 
        return VertexCategory::Start; //Interior angle < Pi => Start
      else  
        return VertexCategory::Split; //Interior angle > Pi => Split
    }
    else if((v_prev->point > vertex->point) && (v_next->point > vertex->point)) {
      //both neighbours are above vertex => End or Merge
      Geom::LineSeg2D seg{v_prev->point, vertex->point};
      if(Geom::LeftOrBeyond(seg,v_next->point))
        return VertexCategory::End;
      else
        return VertexCategory::Merge;  
    }
    else {
      return VertexCategory::Regular;
    }
  }

  void MonotonePartitionAlgo::InitialiseEventQueue()
  {
    uint32_t i = 0;
    for(auto v : m_polygon.GetVertices()) {
      SPG_ASSERT(v != nullptr);
      Event e;
      e.tag = v->tag;        
      e.vertex = v;
      e.vertex_category = GetVertexCategory(v);
      m_event_queue.push_back(e);
    }
    m_event_queue_unsorted = m_event_queue;
    //sort in reverse order so that highest priority event at the end (no pop_front() method on vectors)
    std::sort(m_event_queue.rbegin(), m_event_queue.rend(), EventComparator());
  }

  void MonotonePartitionAlgo::Step()
  {
    Event e = m_event_queue.back(); //highest priority at back
    m_cur_event_point = e.vertex->point;
    PrintEvent(e);
    SPG_ASSERT(e.vertex_category != VertexCategory::Invalid);
    m_event_queue.pop_back();
    switch(e.vertex_category) {
      case VertexCategory::Start  : HandleStartVertex(e); break;
      case VertexCategory::End    : HandleEndVertex(e); break;
      case VertexCategory::Merge  : HandleMergeVertex(e); break;
      case VertexCategory::Split  : HandleSplitVertex(e); break;
      case VertexCategory::Regular: HandleRegularVertex(e); break;
      default:
        SPG_ERROR("Event with invalid category"); return;
    }
    PrintStatusStucture();
    PrintDiagonals();
  }

  void MonotonePartitionAlgo::MakeMonotone()
  {
    while(!m_event_queue.empty()) {
      Step();
    }
    for(auto& diagonal : m_monotone_diagonals) {
      m_polygon.Split(diagonal.first, diagonal.second);
    }
  }

  std::vector<Point2d> MonotonePartitionAlgo::GetDiagonalEndPoints(DiagonalList& diagonal_list)
  {
    std::vector<Point2d> points;
    for(auto& diagonal : diagonal_list) {
      points.push_back(diagonal.first->point);
      points.push_back(diagonal.second->point);
    }
    return points;
  }

  void MonotonePartitionAlgo::HandleStartVertex(const Event& e)
  {
    DCEL::HalfEdge*  half_edge = GetDepartingEdge(e.vertex);
    SPG_ASSERT(half_edge != nullptr);
    const auto [it, success] = m_T.insert({*half_edge, e});
    SPG_ASSERT(success);
  }

  void MonotonePartitionAlgo::HandleEndVertex(const Event& e)
  {
    SPG_ASSERT(m_T.size() > 0);
    DCEL::HalfEdge*  half_edge = GetDepartingEdge(e.vertex);
    SPG_ASSERT(half_edge != nullptr);
    SPG_ASSERT(half_edge->prev != nullptr);
    auto itr = m_T.find(*(half_edge->prev));
    SPG_ASSERT(itr != m_T.end());
    HelperPoint helper = itr->second;
    if(helper.vertex_category == VertexCategory::Merge) {
      SPG_ASSERT(m_polygon.DiagonalCheck(e.vertex, helper.vertex).is_valid);
      m_monotone_diagonals.push_back({e.vertex, helper.vertex});
    }
    m_T.erase(itr); 
  }

  void MonotonePartitionAlgo::HandleSplitVertex(const Event& e)
  {
    //Search m_T to find edge directly left of e.vertex
    SPG_ASSERT(m_T.size() > 0);
    DCEL::HalfEdge*  cur_edge = GetDepartingEdge(e.vertex);
    //Get first element not less than (i.e. >=) key. If no such element found, return end()
    auto itr = m_T.lower_bound(*cur_edge);   
    //Should never return begin(), even if there is only 1 element in m_T.  It may return end(), in which case prev(itr) should be valid (i.e. the last element in m_T).  We've already checked that m_T has at least 1 element
    SPG_ASSERT(itr != m_T.begin()); 
    itr = std::prev(itr); //theoretically points to edge directly left of e.vertex in m_T
    HelperPoint helper = itr->second;
    //Insert diagonal connecting e.vertex to event_point.helper_vertex
    SPG_ASSERT(m_polygon.DiagonalCheck(e.vertex, helper.vertex).is_valid);
    m_monotone_diagonals.push_back({e.vertex, helper.vertex});
    //helper(ej -> vi)
    auto [it, inserted] = m_T.insert_or_assign(itr->first,e);
    SPG_ASSERT(!inserted); //Should have been assigned, not inserted
    { //insert e_i into T, helper e_i -> v_i
      DCEL::HalfEdge*  half_edge = GetDepartingEdge(e.vertex);
      auto [it, success] = m_T.insert({*half_edge, e});
      SPG_ASSERT(success);
    }
  }

  void MonotonePartitionAlgo::HandleMergeVertex(const Event& e)
  {
    //first part is same as for end vertex
    DCEL::HalfEdge*  half_edge = GetDepartingEdge(e.vertex);
    SPG_ASSERT(half_edge != nullptr);
    SPG_ASSERT(half_edge->prev != nullptr);
    auto itr = m_T.find(*(half_edge->prev));
    SPG_ASSERT(itr != m_T.end()); 
    
    HelperPoint helper = itr->second; 
    if(helper.vertex_category == VertexCategory::Merge) {
      SPG_ASSERT(m_polygon.DiagonalCheck(e.vertex, helper.vertex).is_valid);
      m_monotone_diagonals.push_back({e.vertex, helper.vertex});
    }
    //delete e-1 from T
    m_T.erase(itr); 

    //Search m_T to find edge directly left of e.vertex
    DCEL::HalfEdge*  cur_edge = GetDepartingEdge(e.vertex);
    itr = m_T.lower_bound(*cur_edge); 
    SPG_ASSERT(itr != m_T.begin());
    itr = std::prev(itr); //theoretically points to element left of e.vertex in m_T
    helper = itr->second;
    if(helper.vertex_category == VertexCategory::Merge) {
      SPG_ASSERT(m_polygon.DiagonalCheck(e.vertex, helper.vertex).is_valid);
      m_monotone_diagonals.push_back({e.vertex, helper.vertex});
    }
    //helper(ej <-vi)
    const auto [it, inserted] = m_T.insert_or_assign(itr->first,e);
    SPG_ASSERT(!inserted); //Should have been assigned, not inserted. 
  }

  void MonotonePartitionAlgo::HandleRegularVertex(const Event& e)
  {
    if(PolygonInteriorOnRight(e.vertex)) {
      //first part is same as for end vertex
      DCEL::HalfEdge*  half_edge = GetDepartingEdge(e.vertex);
      SPG_ASSERT(half_edge != nullptr);
      SPG_ASSERT(half_edge->prev != nullptr);
      auto itr_e_prev = m_T.find(*(half_edge->prev));
      SPG_ASSERT(itr_e_prev != m_T.end()); 
      
      HelperPoint helper_e_prev = itr_e_prev->second; 
      if(helper_e_prev.vertex_category == VertexCategory::Merge) {
        SPG_ASSERT(m_polygon.DiagonalCheck(e.vertex, helper_e_prev.vertex).is_valid);
        m_monotone_diagonals.push_back({e.vertex, helper_e_prev.vertex});
      }
      m_T.erase(itr_e_prev); //return itr to element following removed element  
      const auto [it, inserted] = m_T.insert_or_assign(*half_edge,e);
      SPG_ASSERT(inserted); //Should have been inserted, not assigned
    }
    else {
      //search in T to find e_j directly left of v_i
      DCEL::HalfEdge*  cur_edge = GetDepartingEdge(e.vertex);
      auto itr = m_T.lower_bound(*cur_edge); 
      SPG_ASSERT(itr != m_T.begin());
      itr = std::prev(itr); //theoretically points to element left of e.vertex in m_T
      auto helper = itr->second;
      if(helper.vertex_category == VertexCategory::Merge) {
        SPG_ASSERT(m_polygon.DiagonalCheck(e.vertex, helper.vertex).is_valid);
        m_monotone_diagonals.push_back({e.vertex, helper.vertex});
      }
      //helper(ej <- vi)
      const auto [it, inserted] = m_T.insert_or_assign(itr->first,e);
      SPG_ASSERT(!inserted); //Should have been assigned, not inserted
    }
  }

  DCEL::HalfEdge* MonotonePartitionAlgo::GetDepartingEdge(DCEL::Vertex* v)
  {
    auto half_edges = m_polygon.GetDepartingEdges(v);
    SPG_ASSERT(half_edges.size() == 2);
    if(half_edges[0]->incident_face->outer != nullptr)
      return half_edges[0];
    else {
      SPG_ASSERT(half_edges[1]->incident_face->outer != nullptr)
      return half_edges[1];
    }
  }

  bool MonotonePartitionAlgo::PolygonInteriorOnRight(DCEL::Vertex* v)
  {
    DCEL::HalfEdge* e = GetDepartingEdge(v);
    auto point_prev = e->prev->origin->point;
    auto point_cur = e->origin->point;
    auto point_next = e->next->origin->point;
    return (point_prev > point_cur) && (point_cur > point_next);
  }

  //Debug logging
  std::string GetCategoryString(VertexCategory category) 
  {
    switch(category) {
      case VertexCategory::Start  : return "Start";
      case VertexCategory::End    : return "End";
      case VertexCategory::Merge  : return "Merge";
      case VertexCategory::Split  : return "Split";
      case VertexCategory::Regular: return "Regular";
      default: return "Invalid";
    }
  }

  void MonotonePartitionAlgo::PrintEvent(Event e)
  {
    auto cat_str = GetCategoryString(e.vertex_category);
    SPG_WARN("Next Event: {}: ({},{}) - {} ----------------", e.tag, e.vertex->point.x,  e.vertex->point.y, cat_str);
  }

  void MonotonePartitionAlgo::PrintStatusStucture()
  {
    SPG_TRACE("Status structure: ------------------- ");
    for(const auto& element : m_T) {
        auto half_edge = element.first;
        auto helper = element.second;
        SPG_TRACE("e{} -> v{}", half_edge.origin->tag, helper.tag);
    }
  }
  
  void MonotonePartitionAlgo::PrintDiagonals()
  {
    SPG_TRACE("Diagonals: ------------------- ");
    for(auto& inter : m_monotone_diagonals) {
      SPG_TRACE(" {}->{}", inter.first->tag, inter.second->tag);
    }
  }

  using DiagonalList =  std::vector<std::pair<DCEL::Vertex*, DCEL::Vertex*>>;
  static void PrintDiags(DiagonalList& diags)
  {
    SPG_TRACE("Diags: ------------------- ");
    for(auto& inter : diags) {
      SPG_TRACE(" {}->{}", inter.first->tag, inter.second->tag);
    }
  }
  static void PrintVertices(std::vector<DCEL::Vertex*>& vertices)
  {
    SPG_TRACE("Face with vertices (sorted): ------------------- ");
    for(auto v : vertices) {
      SPG_TRACE(" {}", v->tag);
    }
  }

  static void PrintStack(std::vector<DCEL::Vertex*>& stack)
  {
    SPG_TRACE("Stack: ------------------- ");
    for(auto v : stack) {
      SPG_TRACE(" {}", v->tag);
    }
  }


  void MonotonePartitionAlgo::Triangulate()
  {
    SPG_TRACE("TRIANGULATING: ------------------- ");
    // if(!m_polygon.Validate())
    //   return;

    for(DCEL::Face* face : m_polygon.GetFaces()) {
      TriangulateFace(face);
    }
    for(auto& diagonal : m_triangulation_diagonals) {
      m_polygon.Split(diagonal.first, diagonal.second);
    }
  }

  void MonotonePartitionAlgo::TriangulateFace(DCEL::Face* face)
   {
      SPG_ASSERT(face != nullptr);
      if(face->outer == nullptr)
        return;

      std::vector<DCEL::Vertex*> vertices = m_polygon.GetVertices(face);
      SPG_ASSERT(vertices.size() >= 3);

      //sort points top to bottom
      std::vector<DCEL::Vertex*> sorted_vertices = vertices;
      std::sort(std::begin(sorted_vertices), std::end(sorted_vertices), 
        [](DCEL::Vertex* lhs, DCEL::Vertex* rhs) {
          return lhs->point > rhs->point;
      });

      DCEL::Vertex* v_top = sorted_vertices[0];
      DCEL::HalfEdge* edge_start = m_polygon.GetDepartingEdge(v_top, face);
      DCEL::HalfEdge* edge = edge_start;
      SPG_ASSERT(edge != nullptr);
      SPG_ASSERT(edge->incident_face == face);

      //PrintVertices(sorted_vertices);

      std::set<DCEL::Vertex*> vertices_chain1, vertices_chain2;
      while(edge->next->origin != sorted_vertices.back()) {
        vertices_chain1.insert(edge->next->origin);
        edge = edge->next;
      }
      edge = edge_start;
      while(edge->prev->origin != sorted_vertices.back()) {
        vertices_chain2.insert(edge->prev->origin);
        edge = edge->prev;
      }
      
      std::vector<DCEL::Vertex*> stack;
      stack.push_back(sorted_vertices[0]);
      stack.push_back(sorted_vertices[1]);

      for(uint32_t i=2; i< sorted_vertices.size()-1; ++i) {
        DCEL::Vertex* v_i = sorted_vertices[i];
        DCEL::Vertex* v_stack_top = stack.back();
        if( (vertices_chain1.contains(v_i) && vertices_chain2.contains(v_stack_top)) ||
            (vertices_chain1.contains(v_stack_top) && vertices_chain2.contains(v_i))) { //different chains
          while(stack.size() != 0) {
            DCEL::Vertex* v_j = stack.back();
            stack.pop_back();
            if(stack.size() >= 1) {
              SPG_ASSERT(m_polygon.DiagonalCheck(v_i, v_j).is_valid);
              m_triangulation_diagonals.push_back({v_i, v_j});
            }
          }
          stack.push_back(v_stack_top);
          stack.push_back(v_i);
        }
        else { //same chain
          SPG_ASSERT(!stack.empty());
          DCEL::Vertex* last_popped = stack.back();
          stack.pop_back();
          while(!stack.empty()) {
            DCEL::Vertex* v_j = stack.back();
            if(!m_polygon.DiagonalCheck(v_i,v_j).is_valid)
              break;
            stack.pop_back();
            last_popped = v_j;
            m_triangulation_diagonals.push_back({v_i, v_j});
          }
          stack.push_back(last_popped);
          stack.push_back(v_i);
        }
      }

      //Add diagonals from v_n to all stack vertices except the first and last
      DCEL::Vertex* v_n = sorted_vertices.back();
      for(uint32_t i=1; i<stack.size()-1; ++i){
        DCEL::Vertex* v_i = stack[i];  
        SPG_ASSERT(m_polygon.DiagonalCheck(v_n, v_i).is_valid);
        m_triangulation_diagonals.push_back({v_n, v_i});
      }
   }

 
}