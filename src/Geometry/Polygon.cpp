#include "Polygon.h"
#include <Common/Common.h>

namespace Geom
{
  namespace SP
  {

    Polygon::Polygon(const std::vector<Point2d>& points)
    {
      const auto size = points.size();
      SPG_ASSERT(size >= 3);

      for(auto& p : points)
        vertices.push_back(new Vertex{p});

      vertices[0]->prev = vertices[size-1];
      for (auto i = 0; i < size; ++i)
      {
        vertices[i]->next = vertices[(i+1)%size];
        if(i > 0)
          vertices[i]->prev = vertices[i-1];
      }
    }

    std::vector<Point2d> Polygon::GetEars()
    {
      std::vector<Point2d> ears;
      for(auto v : vertices) {
        if(v->is_ear)
          ears.push_back(v->point);    
      }
      return ears;
    }

  }

  namespace DCEL
  {
    //Vid 29
    Polygon::Polygon(std::vector<Point2d>& points)
    {
      if(points.size() < 3)
        return;

      for(auto &p : points) {
        m_vertices.push_back(new Vertex(p));
      }

      for(auto i=0; i<m_vertices.size()-2; ++i) {
        auto edge = new Edge(m_vertices[i]);
        auto edge_twin = new Edge(m_vertices[i+1]);
        m_vertices[i]->incident_edge = edge;
        edge->twin = edge_twin;
        m_edges.push_back(edge);
        m_edges.push_back(edge_twin);
      }

      auto edge = new Edge(m_vertices.back());
      auto edge_twin = new Edge(m_vertices.front());
      edge->twin = edge_twin;
      edge_twin->twin = edge;
      m_edges.push_back(edge_twin);

      m_vertices[m_vertices.size() - 1]->incident_edge = edge;

      // Set the prev and next for the element middle of the list (2 : size-2)
      for(auto i = 2; i< m_edges.size()-3; i++)   {
        if(i%2 == 0) { // even case - CCW edges
          m_edges[i]->next = m_edges[i+2];
          m_edges[i]->prev = m_edges[i-2];
        } 
        else { //odd case - CW edges
          m_edges[i]->next = m_edges[i-2];
          m_edges[i]->prev = m_edges[i+2];
        }
      }

      m_edges[0]->next = m_edges[2];
      m_edges[0]->prev = m_edges[m_edges.size()-2];
      m_edges[1]->next = m_edges[m_edges.size()-1];
      m_edges[1]->prev = m_edges[m_edges.size()];

      m_edges[m_edges.size()-2]->next = m_edges[0];
      m_edges[m_edges.size()-2]->prev = m_edges[m_edges.size()-4];
      m_edges[m_edges.size()-1]->next = m_edges[m_edges.size()-3];
      m_edges[m_edges.size()-1]->prev = m_edges[1];

      //setup faces
      Face* f1 = new Face();
      Face* f2 = new Face();

      f1->outer = m_edges[0]; //CCW edge list
      f2->inner.push_back(m_edges[1]); //f2 is an unbounded face which wraps f1.  So f1 is a hole in f2 - CW edge list
      m_faces.push_back(f1);
      m_faces.push_back(f2);

      f1->outer->incident_face = f1;
      edge = f1->outer->next;
      while(edge != f1->outer) {
        edge->incident_face = f1;
        edge = edge->next;
      }

      //f2-> inner has half edges connected in CW order
      f2->inner[0]->incident_face = f2;
      edge = f2->inner[0]->next;
      while(edge != f2->inner[0]) {
        edge->incident_face = f2;
        edge = edge->next;
      }
    }

    Vertex* Polygon::GetVertex(Point2d point)
    {
      for (size_t i = 0; i < m_vertices.size(); i++) {
        if (point == m_vertices[i]->point)
          return m_vertices[i];
      }
      return nullptr;
    }

    //Vid 30
     void Polygon::GetEdgesWithSamefaceAndGivenOrigins(Vertex* v1, Vertex* v2, Edge** edge_leaving_v1, Edge** edge_leaving_v2)
     {
        std::vector<Edge*> edges_with_v1_ori, edges_with_v2_ori;

        // Get all the edges with orgin v1
        auto v1_inci_edge = v1->incident_edge;
        edges_with_v1_ori.push_back(v1_inci_edge);

        auto next_edge = v1_inci_edge->twin->next;
		    while (next_edge != v1_inci_edge) {
          edges_with_v1_ori.push_back(next_edge);
          next_edge = next_edge->twin->next;
		    }

        // Get all the edges with orgin v2
        auto v2_inci_edge = v2->incident_edge;
        edges_with_v2_ori.push_back(v2_inci_edge);

        next_edge = v2_inci_edge->twin->next;
        while (next_edge != v2_inci_edge) {
          edges_with_v2_ori.push_back(next_edge);
          next_edge = next_edge->twin->next;
        }

        // Get two edges, one with origin v1 and other with origin v2 and incident to same face
        for ( auto ev1 : edges_with_v1_ori){
          for ( auto ev2 : edges_with_v2_ori){
            if (ev1->incident_face->outer != nullptr) {
              if (ev1->incident_face == ev2->incident_face) {
                *edge_leaving_v1 = ev1;
                *edge_leaving_v2 = ev2;
                return;
              }
            }
          }
        }
     }

    //Vid 30
    //Insert Edge between vertices v1 and v2
    bool Polygon::Split(Vertex* v1, Vertex* v2)
    {
      // Find two edges with given points as origins and are in same face.
      Edge* edge_oriV1;
      Edge* edge_oriV2;
		  GetEdgesWithSamefaceAndGivenOrigins(v1, v2, &edge_oriV1, &edge_oriV2);    

      if (edge_oriV1->id == -1 || edge_oriV2->id == -1)
			  return false;						// Cannot find a edges with same face with ori _v1, _v2

      // If the vertices are adjucent we can return.
      if (edge_oriV1->next->origin == v2 || edge_oriV1->prev->origin == v2)
        return false;

      // Later we can delete this entry
		  Face* previous_face = edge_oriV1->incident_face;  

      auto half_edge1 = new Edge(v1);
		  auto half_edge2 = new Edge(v2);

      half_edge1->twin = half_edge2;
      half_edge2->twin = half_edge1;
      half_edge1->next = edge_oriV2;
      half_edge2->next = edge_oriV1;

      half_edge1->prev = edge_oriV1->prev;
      half_edge2->prev = edge_oriV2->prev;

      half_edge1->next->prev = half_edge1;
      half_edge2->next->prev = half_edge2;
      half_edge1->prev->next = half_edge1;
      half_edge2->prev->next = half_edge2;

      Face* new_face1 = new Face();
      new_face1->outer = half_edge1;
      half_edge1->incident_face = new_face1;
      auto temp_edge = half_edge1->next;
      while (temp_edge != half_edge1) {
        temp_edge->incident_face = new_face1;
        temp_edge = temp_edge->next;
      }

      Face* new_face2 = new Face();
      new_face2->outer = half_edge2;
      half_edge2->incident_face = new_face2;
      temp_edge = half_edge2->next;
      while (temp_edge != half_edge2) {
        temp_edge->incident_face = new_face2;
        temp_edge = temp_edge->next;
      }

      m_faces.push_back(new_face1);
		  m_faces.push_back(new_face2);

		  auto itr = std::find(m_faces.begin(), m_faces.end(), previous_face);

      if (itr != m_faces.end()) {
        m_faces.erase(itr);
        delete previous_face;
      }

		  return true;
    }

    //Join the 2 faces seperated by Edge between v1 and v2
    bool Polygon::Join(Vertex* v1, Vertex* v2)
    {
      return false;
    }
  }
}