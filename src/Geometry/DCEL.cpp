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
      else {
        h->prev = (i+2 >= m_half_edges.size() ? m_half_edges[(i+2) - m_half_edges.size()] : m_half_edges[i+2] );
        h->next = (i-2 < 0 ? m_half_edges[m_half_edges.size() + (i-2)] : m_half_edges[i-2] );
      }
    }

    //faces
    tag = 0;
    Face* face = new Face();
    face->tag = (tag++);
    face->outer = *(m_half_edges.begin()); 

    Face* face_unbound = new Face();
    face_unbound->tag = (tag++);
    face_unbound->outer = nullptr;
    face_unbound->inner.push_back((*(m_half_edges.begin()+1)));

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

  bool DCEL::IsInteriorOnRight(Vertex* v, Face* f)
  {
    return false;
  }

  void DCEL::Validate()
  {
      //Vertices
      for(Vertex* v : m_vertices) {
        SPG_ASSERT(v != nullptr);
        HalfEdge* e_first = v->incident_edge; 
        SPG_ASSERT(e_first != nullptr);
        //v doesn't need to be the origin of incident_edge, although here it is setup to be like this
        if(e_first->origin != v)
          e_first = e_first->twin;
        SPG_ASSERT(e_first->origin == v) ;
        HalfEdge* e = e_first;
        uint32_t iters = 0;
        do {
          e = e->twin;
          SPG_ASSERT(e != nullptr);
          e = e->next;
          SPG_ASSERT(e->origin == v);
          SPG_ASSERT(e != nullptr);
          SPG_ASSERT(++iters < 10);
        } while (e != e_first);
      }

      SPG_ASSERT(m_half_edges.size() == 2*m_vertices.size());

      for(HalfEdge* e : m_half_edges)
      {
        SPG_ASSERT(e->origin != nullptr);
        SPG_ASSERT(e->next != nullptr);
        SPG_ASSERT(e->prev != nullptr);
        SPG_ASSERT(e->twin != nullptr);
        SPG_ASSERT(e->twin->twin == e);
        SPG_ASSERT(e->incident_face != nullptr);

        SPG_ASSERT(e->prev->twin != nullptr);
        SPG_ASSERT(e->prev->twin->origin == e->origin);
        SPG_ASSERT(e->twin->twin->origin == e->origin);

        if(e->incident_face->outer != nullptr) {
          SPG_ASSERT(e->incident_face == e->incident_face->outer->incident_face);
        }
        
      }

      for(Face* f : m_faces) {
        SPG_ASSERT( (f->outer != nullptr) || ( (f->inner.size()>=1) && f->inner[0] != nullptr));

        if(f->outer != nullptr) {
          HalfEdge* e = f->outer;
          SPG_ASSERT(e->incident_face == f);
          uint32_t iters = 0;
          do {
            e = e->next;
            SPG_ASSERT(e->incident_face == f);
            SPG_ASSERT(++iters < 100);
          } while(e != f->outer);
        }

        if(f->inner.size()>=1) {
          HalfEdge* e = f->inner[0];
            SPG_ASSERT(e != nullptr);
          SPG_ASSERT(e->incident_face == f);
          uint32_t iters = 0;
          do {
            e = e->next;
            SPG_ASSERT(e->incident_face == f);
            SPG_ASSERT(++iters < 100);
          } while(e != f->inner[0]);
        }

      }
      SPG_INFO("DCEL structure validated - no guarantees though!");
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
    return nullptr;
  }

  auto DCEL::FindDepartingEdgesWithCommonFace(Vertex* v1, Vertex* v2) -> std::pair<HalfEdge*,HalfEdge*>
  {
    auto departing_edges_v1 = GetDepartingEdges(v1);
    auto departing_edges_v2 = GetDepartingEdges(v2);

    for(auto e1 : departing_edges_v1) {
      Face* f1 = e1->incident_face;
      for(auto e2 : departing_edges_v2) {
        Face* f2 = e2->incident_face;
        if(f1 == f2)
          return std::make_pair(e1,e2);
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

    //interior/exterior check based on CCW otientation
    if(IsConvex(orig, orig_depart_edge)) { //The 2 neighbours must be on different sides of candidate line
      bool interior = Right(candidate_seg, v_next) && Left(candidate_seg, v_prev);
      return interior;
    }
    //orig is a reflex vert.  
    bool exterior = Left(candidate_seg, v_next) && Right(candidate_seg, v_prev);
    return !exterior;
  }

  DiagonalData DCEL::DiagonalCheck(Vertex* v1, Vertex* v2)
  {
    DiagonalData diagonal_data{nullptr,nullptr,false};
    if(v1 == v2)
      return diagonal_data;
          
    auto [e1_depart, e2_depart] = FindDepartingEdgesWithCommonFace(v1,v2);
    if( (e1_depart == nullptr) || ((e2_depart == nullptr)))
      return diagonal_data;

    //Check if v1 and v2 are neighbours
    if(e1_depart->next->origin == v2)
      return diagonal_data;
    if(e2_depart->next->origin == v1)
      return diagonal_data;

    if(AnyIntersectionsExist(v1, v2, e1_depart))
      return diagonal_data;

    diagonal_data.is_valid = MakesInteriorConnection(v1,v2,e1_depart) && 
        MakesInteriorConnection(v2,v1,e2_depart);

    diagonal_data.departing_edge_v1 = e1_depart;
    diagonal_data.departing_edge_v2 = e2_depart;   

    return diagonal_data;
  }

  void DCEL::Split(Vertex* v1, Vertex* v2)
  {
    DiagonalData diagonal_data = DiagonalCheck(v1,v2);
    if(!diagonal_data.is_valid)
      return;
    
    //2 new half hedges, 1 new face
    HalfEdge* e1 = new HalfEdge();
    HalfEdge* e2 = new HalfEdge();
    Face* f1 = diagonal_data.departing_edge_v1->incident_face;
    Face* f2 = new Face();

    e1->origin = v1;
    e1->next = diagonal_data.departing_edge_v2;
    e1->prev = diagonal_data.departing_edge_v1->prev;
    e1->twin = e2;
    e1->incident_face = f1;
    
    e2->origin = v2;
    e2->next = diagonal_data.departing_edge_v1;
    e2->prev = diagonal_data.departing_edge_v2->prev;
    e2->twin = e1;
    e2->incident_face = f2;

    diagonal_data.departing_edge_v1->prev->next = e1;
    diagonal_data.departing_edge_v1->prev = e2;

    diagonal_data.departing_edge_v2->prev->next = e2;
    diagonal_data.departing_edge_v2->prev = e1;

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


}
