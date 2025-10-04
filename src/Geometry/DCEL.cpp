#include "DCEL.h"
#include <Common/Common.h>

namespace Geom
{
  using Vertex = DCEL::Vertex;
  using HalfEdge = DCEL::HalfEdge;
  using Face = DCEL::Face;
  using DiagonalData = DCEL::DiagonalData;

  DCEL::DCEL(const std::vector<Point2d>& points)
  {
    Set(points);
  }

  DCEL::~DCEL()
  {
    Clear();
  }
  
  void DCEL::Clear()
  {
    for(Vertex* vert : m_vertices) {
      if(vert)
        delete vert;
      vert = 0;  
    }
    for(HalfEdge* h : m_half_edges) {
      if(h)
        delete h;
      h = 0;  
    }   
    for(Face* f : m_faces) {
      if(f)
        delete f;
      f = 0;  
    }
    m_faces.clear();
    m_half_edges.clear();
    m_vertices.clear();
  }

  //Assumes that points ordered CCW
  void DCEL::Set(const std::vector<Point2d>& points)
  {
    SPG_ASSERT(m_vertices.size() == 0);
    SPG_ASSERT(m_half_edges.size() == 0);
    SPG_ASSERT(m_faces.size() == 0);

    if(points.size() < 3)
      return;

    int32_t tag = 0;
    for(auto& p : points) {
      Vertex* v = new Vertex();
      v->point  = p;
      v->tag = (tag++);
      m_vertices.push_back(v);
    }

    tag = 0;
    for(size_t i = 0; i<m_vertices.size(); ++i) {
      HalfEdge* h1 = new HalfEdge();
      HalfEdge* h2 = new HalfEdge();
      h1->tag = (tag++);
      h2->tag = (tag++);
      Vertex* v_cur = m_vertices[i];
      Vertex* v_next = (i+1 < m_vertices.size() ? m_vertices[i+1] : m_vertices[0]);
      h1->origin = v_cur;
      h2->origin = v_next;
      v_cur->incident_edge = h1;
      h1->twin = h2;
      h2->twin = h1;
      m_half_edges.push_back(h1);
      m_half_edges.push_back(h2);
    }
    
    //CCW half edges in even indices in m_half_edges. CW half_edges in odd indices
    for(int i = 0; i < m_half_edges.size(); ++i) {
      HalfEdge* h = m_half_edges[i];
      if(i%2 == 0) { //CCW
        h->next = (i+2 >= m_half_edges.size() ? m_half_edges[(i+2) - m_half_edges.size()] : m_half_edges[i+2] );
        h->prev = (i-2 < 0 ? m_half_edges[m_half_edges.size() + (i-2)] : m_half_edges[i-2] );
      }
      else { //CW
        h->prev = (i+2 >= m_half_edges.size() ? m_half_edges[(i+2) - m_half_edges.size()] : m_half_edges[i+2] );
        h->next = (i-2 < 0 ? m_half_edges[m_half_edges.size() + (i-2)] : m_half_edges[i-2] );
      }
    } 

    //faces
    tag = 0;
    Face* face = new Face();
    face->tag = (tag++);
    face->outer = *(m_half_edges.begin()); //first CCW edge

    Face* face_unbound = new Face();
    face_unbound->tag = (tag++);
    face_unbound->outer = nullptr;
    face_unbound->inner.push_back((*(m_half_edges.begin()+1))); //first CW edge

    for(size_t i=0; i< m_half_edges.size(); i+=2) {
      HalfEdge* e = m_half_edges[i];
      e->incident_face = face;
    }
    for(size_t i=1; i< m_half_edges.size(); i+=2) {
      HalfEdge* e = m_half_edges[i];
      e->incident_face = face_unbound;
    }
    m_faces.push_back(face);
    m_faces.push_back(face_unbound);
  }

  Vertex* DCEL::GetVertex(int32_t tag)
  {
    for(auto v : m_vertices) {
      if(v->tag == tag)
        return v;
    }
    SPG_ASSERT(false);
    return nullptr;
  }

  #define DCEL_VALIDATE(check) \
  if(!(check)) { \
      SPG_ERROR("DCEL validation failed. File: {}, Line {}", __FILE__, __LINE__); \
      return false; \
  }

  bool DCEL::Validate() const
  {
      SPG_INFO("Validating DCEL");
      //Vertices
      for(Vertex* v : m_vertices) {
        DCEL_VALIDATE(v != nullptr);
        HalfEdge* e_first = v->incident_edge; 
        DCEL_VALIDATE(e_first != nullptr);
        //v doesn't need to be the origin of incident_edge, although here it is setup to be like this
        if(e_first->origin != v)
          e_first = e_first->twin;
        DCEL_VALIDATE(e_first->origin == v) ;
        HalfEdge* e = e_first;
        uint32_t iters = 0;
        do {
          e = e->twin;
          DCEL_VALIDATE(e != nullptr);
          e = e->next;
          DCEL_VALIDATE(e->origin == v);
          DCEL_VALIDATE(e != nullptr);
          DCEL_VALIDATE(++iters < 10);
        } while (e != e_first);
      }

      //Doesn't apply once diagonals have been added
      //DCEL_VALIDATE(m_half_edges.size() == 2*m_vertices.size());

      for(HalfEdge* e : m_half_edges)
      {
        DCEL_VALIDATE(e->origin != nullptr);
        DCEL_VALIDATE(e->next != nullptr);
        DCEL_VALIDATE(e->prev != nullptr);
        DCEL_VALIDATE(e->twin != nullptr);
        DCEL_VALIDATE(e->twin->twin == e);
        DCEL_VALIDATE(e->incident_face != nullptr);

        DCEL_VALIDATE(e->prev->twin != nullptr);
        DCEL_VALIDATE(e->prev->twin->origin == e->origin);
        DCEL_VALIDATE(e->twin->twin->origin == e->origin);

        if(e->incident_face->outer != nullptr) {
          DCEL_VALIDATE(e->incident_face == e->incident_face->outer->incident_face);
        }
        
      }

      for(Face* f : m_faces) {
        DCEL_VALIDATE( (f->outer != nullptr) || ( (f->inner.size()>=1) && f->inner[0] != nullptr));

        if(f->outer != nullptr) {
          HalfEdge* e = f->outer;
          DCEL_VALIDATE(e->incident_face == f);
          uint32_t iters = 0;
          do {
            e = e->next;
            DCEL_VALIDATE(e->incident_face == f);
            DCEL_VALIDATE(++iters < 100);
          } while(e != f->outer);
        }

        if(f->inner.size()>=1) {
          HalfEdge* e = f->inner[0];
          DCEL_VALIDATE(e != nullptr);
          DCEL_VALIDATE(e->incident_face == f);
          uint32_t iters = 0;
          do {
            e = e->next;
            DCEL_VALIDATE(e->incident_face == f);
            DCEL_VALIDATE(++iters < 100);
          } while(e != f->inner[0]);
        }

      }
      SPG_INFO("DCEL structure validated!");
      return true;
  }

  std::vector<HalfEdge*> DCEL::GetDepartingEdges(Vertex* v)
  {
    HalfEdge* e_first = v->incident_edge; 
    if(e_first->origin != v)
      e_first = e_first->twin;
    HalfEdge* e = e_first;
    std::vector<HalfEdge*> half_edges;
    do {
      half_edges.push_back(e);
      e = e->twin;
      e = e->next;
    } while (e != e_first);

    return half_edges;
  }

  HalfEdge* DCEL::GetDepartingEdge(Vertex* v, Face* f) 
  {
    auto departing_edges = GetDepartingEdges(v);
    for(auto half_edge : departing_edges) {
      if(half_edge->incident_face == f)
        return half_edge;
    }
    return nullptr;
  }

  //The common face must be bounded so that the edges are oriented CCW
  auto DCEL::FindDepartingEdgesWithCommonFace(Vertex* v1, Vertex* v2) -> std::pair<HalfEdge*,HalfEdge*>
  {
    auto departing_edges_v1 = GetDepartingEdges(v1);
    auto departing_edges_v2 = GetDepartingEdges(v2);

    for(auto e1 : departing_edges_v1) {
      Face* f1 = e1->incident_face;
      SPG_ASSERT(f1 != nullptr);
      for(auto e2 : departing_edges_v2) {
        Face* f2 = e2->incident_face;
        SPG_ASSERT(f2 != nullptr);
        if((f1 == f2) && (f1->outer != nullptr) && (f2->outer != nullptr)) {
          return std::make_pair(e1,e2);
        } 
      }
    }
    return std::make_pair(nullptr,nullptr);
  }
  
  bool DCEL::AnyIntersectionsExist(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge)
  {
    LineSeg2D test_seg = {orig->point,dest->point};
    HalfEdge* e = orig_depart_edge->next;
    while (e != orig_depart_edge->prev) {
      if( (e->origin == dest) || (e->twin->origin == dest) ) {
        e = e->next;  
        continue;
      }
      LineSeg2D cur_seg = GetLineSeg2d(e);
      if(Geom::IntersectionExists(test_seg, cur_seg)) 
        return true;
      e = e->next;  
    } 
    return false;
  }

  bool DCEL::IsConvex(Vertex* v, HalfEdge* departing_edge)
  {
    SPG_ASSERT(v == departing_edge->origin);
    Vertex* v_next = departing_edge->next->origin;
    Vertex* v_prev = departing_edge->prev->origin;
    
    LineSeg2D seg_out = GetLineSeg2d(departing_edge);
    LineSeg2D seg_in = GetLineSeg2d(departing_edge->prev);

    return LeftOrBeyond(seg_in, seg_out.end);
  }

  bool DCEL::MakesInteriorConnection(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge)
  {
    Point2d v_next = GetDestinationPoint(orig_depart_edge);
    Point2d v_prev = GetOriginPoint(orig_depart_edge->prev);
    LineSeg2D candidate_seg = GetLineSeg2d(orig,dest);

    //interior/exterior check based on CCW orientation
    if(IsConvex(orig, orig_depart_edge)) { //The 2 neighbours must be on different sides of candidate line
      bool interior = Right(candidate_seg, v_next) && Left(candidate_seg, v_prev);
      return interior;
    }
    //orig is a reflex vert.  To be exterior, next must be on the left and prev must be on the right - can't just be on opposite sides
    bool exterior = Left(candidate_seg, v_next) && Right(candidate_seg, v_prev);
    return !exterior;
  }

  DiagonalData DCEL:: DiagonalCheck(Vertex* v1, Vertex* v2)
  {
    DiagonalData d{nullptr,nullptr,false};
    if(v1 == v2)
      return d;
          
    auto [e1_depart, e2_depart] = FindDepartingEdgesWithCommonFace(v1,v2);
    if( (e1_depart == nullptr) || ((e2_depart == nullptr)))
      return d;

    //Check if v1 and v2 are neighbours
    if(e1_depart->next->origin == v2)
      return d;
    if(e2_depart->next->origin == v1)
      return d;

    //According to ChatGPT need to check both cases here to ensure symmetry?? 
    if(AnyIntersectionsExist(v1, v2, e1_depart) || AnyIntersectionsExist(v2, v1, e2_depart))
      return d;

    d.is_valid = MakesInteriorConnection(v1,v2,e1_depart) && 
        MakesInteriorConnection(v2,v1,e2_depart);

    d.departing_edge_v1 = e1_depart;
    d.departing_edge_v2 = e2_depart;   

    return d;
  }

  void DCEL::Split(Vertex* v1, Vertex* v2)
  {
    DiagonalData d = DiagonalCheck(v1,v2);
    if(!d.is_valid)
      return;
    //diagonal_data stores the departing edge of v1 and v2 respectively that share the same incident face
    
    //2 new half hedges, 1 new face
    //Todo - tags for these
    HalfEdge* e1 = new HalfEdge();
    HalfEdge* e2 = new HalfEdge();
    Face* f1 = d.departing_edge_v1->incident_face;
    Face* f2 = new Face();

    e1->origin = v1;
    e1->next = d.departing_edge_v2;
    e1->prev = d.departing_edge_v1->prev;
    e1->twin = e2;
    e1->incident_face = f1;
    
    e2->origin = v2;
    e2->next = d.departing_edge_v1;
    e2->prev = d.departing_edge_v2->prev;
    e2->twin = e1;
    e2->incident_face = f2;

    d.departing_edge_v1->prev->next = e1;
    d.departing_edge_v1->prev = e2;

    d.departing_edge_v2->prev->next = e2;
    d.departing_edge_v2->prev = e1;

    HalfEdge* e_cur = e2;
    do {
      e_cur->incident_face = f2;
      e_cur = e_cur->next;
    } while(e_cur != e2);
    
    f1->outer = e1;
    f2->outer = e2;
    f2->inner = f1->inner;

    m_half_edges.push_back(e1);
    m_half_edges.push_back(e2);
    m_faces.push_back(f2);
  }

  std::vector<Vertex*> DCEL::GetVertices(Face* face)
  {
    std::vector<Vertex*> vertices;
    SPG_ASSERT(face != nullptr);

    if(face->outer == nullptr)
      return vertices;

    Vertex* v_start = face->outer->origin;
    vertices.push_back(v_start);

    SPG_ASSERT(face->outer->next != nullptr);
    HalfEdge* next_edge = face->outer->next;
    
    while(next_edge->origin != v_start) {
      SPG_ASSERT(next_edge->origin != nullptr); 
      vertices.push_back(next_edge->origin);
      next_edge = next_edge->next;
      SPG_ASSERT(next_edge != nullptr);
      SPG_ASSERT(vertices.size() < 50);
    }
    SPG_ASSERT(vertices.size() >= 3);
    return vertices;
  }

  void DCEL::PrintVertices()
  {
    SPG_WARN("VERTICES ---------");
    SPG_WARN("Idx, Point,  PtVal,  IncEdge");
    for( Vertex*  v  : m_vertices) {
      SPG_TRACE("V{}: ({},{}) E.i{}", v->tag, v->point.x, v->point.y, v->incident_edge->tag);
    }
  }

  void DCEL::PrintHalfEdges()
  {
    SPG_WARN("HALF EDGES ---------");
    SPG_WARN("Idx, Origin, Next, Prev, Twin, Face");
    for(const HalfEdge* e : m_half_edges) {
      SPG_TRACE("E{} V{}->V{}  E.p:{} E.n:{} E.t:{} F.o:{}", e->tag, e->origin->tag, e->next->origin->tag, e->prev->tag, e->next->tag, e->twin->tag, e->incident_face->tag);

    }
  }

  void DCEL::PrintFace(Face* f)
    {
      SPG_WARN("F{}", f->tag);
      SPG_WARN("Outer Loop:");
      if(f->outer != nullptr) {
      HalfEdge* e = f->outer;
      do {
        SPG_TRACE("E{} V{}->V{}", e->tag, e->origin->tag, e->next->origin->tag);
        e = e->next;
      } while(e != f->outer);
      }
      for(HalfEdge* e_start : f->inner) {
      SPG_WARN("Inner Loop:");
        HalfEdge* e = e_start;
        do {
        SPG_TRACE("E{} V{}->V{}", e->tag, e->origin->tag, e->next->origin->tag);
        e = e->next;
      } while(e != e_start);
      }
    }

  void DCEL::PrintFaces()
  {
    SPG_WARN("FACES ---------");
    for(Face* f : m_faces) {
      PrintFace(f);
    }
  }

  void DCEL::Test()
  {
     std::vector<Geom::Point2d> mt_poly_points =
    {
      {16.42f,12.51f},  //A 1
      {13.95,10.36},    //B 2
      {11.2,18.4},      //C 3
      {9.2,16.4},       //D 4
      {6.6,17.8},       //E 5
      {4,16},           //F 6
      {6.62,13.16},     //G 7
      {5.52,9.06},      //H 8
      {3.38,11.36},     //I 9
      {2.54,6.49},      //J 10
      {6.04,3.49},      //K 11
      {8.99,5.24},      //L 12
      {12,2},           //M 13
      {12.26,7.79},     //N 14
      {17.04,6.99}      //O 15
    };

    // Geom::DCEL mt_poly = Geom::DCEL(mt_poly_points);
    // Geom::MonotonePartitionAlgo monotone_spawner(mt_poly_points);
    // monotone_spawner.MakeMonotone();
    // Geom::DCEL& partitioned_polygon = monotone_spawner.GetDCEL();
    // partitioned_polygon.PrintFaces();
    // partitioned_polygon.PrintVertices();
    // partitioned_polygon.PrintHalfEdges();
  }
}
