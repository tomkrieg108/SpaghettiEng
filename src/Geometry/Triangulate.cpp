#include "Triangulate.h"
#include "GeomUtils.h"

namespace Geom
{
  /*
    The below is based on a CCW polygon representation 

    'Diagonal' 2 non-adjacent vertices when connected form a line segment that is fully inside the polygon, and does not intersect with any edges.  This does not depend on depend on interior angle being convex or not.  Diagonals can form on any 2 vertices regardless of the convexity of the interior angle od the end point vertices.

    If the start vertex has convex interior angle, it's neighbour vertices must lie on opposite sides of the diagonal 
    If the start vertex has reflex interior angle, the connecting line between its neighbours is *exterior* to the polygon only if its next neighbour is to the left of the line and its prev neighbour is to the right.  If both these conditions are *not* met, then the connecting line is interior to the polygon, which is what's needed for a diagonal.

    Diagonal check:
      -The connecting line should not intersect with any other edges
      - If start vertex of the line has convex interior angle, then its 2 neighbours should lie on different sides of the connecting line
      -If the start vertex of the line has a reflex interior angle, then it should not be an exterior line, which means its next neighbour should not be on the left of the connecting line, and previous neighbout should not be on the right of the connecting line.

      -Do the same 2 checks on the end vertex
    

    For a vertex to be an ear:
      - Must form a convex interior angle
      - The 2 neighbouring vertices should form a diagonal when connected.  
  */

 // Vid 26 (diagonal test)
  static bool InteriorCheck(const SP::Vertex* v1, const SP::Vertex* v2) 
  {
    if(LeftOrBeyond({v1->point, v1->next->point}, v1->prev->point)) {
          //v1 has convex interior angle
          return Left({v1->point, v2->point}, v1->prev->point) &&
                Left({v2->point, v1->point}, v1->next->point);
    }
    //v1 has reflex interior angle. Check it is not exterior - like this:
    return !(LeftOrBeyond({v1->point, v2->point}, v1->next->point) &&
            LeftOrBeyond({v2->point, v1->point}, v1->prev->point));
  }

  bool IsDiagonal(const SP::Vertex* v1, const SP::Vertex* v2, const PolygonSimple* polygon)
  {
    bool prospect = true;
    std::vector<SP::Vertex*> vertices;

    if(polygon != nullptr)
      vertices = polygon->vertices;
    else {
      auto vertex_ptr = v1->next;
      vertices.push_back((SP::Vertex*)v1);
      while(vertex_ptr != v1) {
        vertices.push_back(vertex_ptr);
        vertex_ptr = vertex_ptr->next;
      }
    }

    SP::Vertex* current = vertices[0];
    SP::Vertex* next;
    do {
      next = current->next;
      if(current != v1 && next != v1 && current != v2 && next != v2 && 
              IntersectionExists({v1->point, v2->point},{current->point, next->point}))  {
        prospect = false;
        break;     
      }
      current = next;
    } while (current != vertices[0]);

    return prospect && InteriorCheck(v1,v2) && InteriorCheck(v2,v1);
  }

  static void InitialiseEarStatus(const PolygonSimple* polygon)
  {
    SP::Vertex *v0, *v1, *v2;
    auto vertices = polygon->vertices;
    v1 = vertices[0];

    do {
      v0 = v1->prev;
      v2 = v1->next;
      if(IsConvex(v0->point,v1->point,v2->point))
        v1->is_ear = IsDiagonal(v0,v2);
      v1 = v1->next;  
    } while (v1 != vertices[0]);
  }

  //vid 27
  // The Levelbuilder program uses a different algo - try that also!
  //edge_list is an output param and is actually the list of diagonals used to triangulate the poly
  void Triangulate_EarClipping(PolygonSimple* polygon, std::vector<SP::Edge>& edge_list)
  {
    InitialiseEarStatus(polygon);
    //return;

    auto vertex_list = polygon->vertices;
    uint32_t vertices_to_process = vertex_list.size();

    SP::Vertex *v0, *v1, *v2, *v3, *v4;
   
    while(vertices_to_process > 3) {

      for(auto i=0; i<vertex_list.size(); ++i) {
        v2 = vertex_list[i];
        if(v2->is_ear && !v2->is_processed) {
          v3 = v2->next;
          v4 = v3->next;
          v1 = v2->prev;
          v0 = v1->prev;

          edge_list.push_back(SP::Edge(*v1,*v3));
          v2->is_processed = true;

          //clip v2
          v1->next = v3;
          v3->prev = v1;

          //update ear status of v1 and v3
          if(IsConvex(v1->prev->point,v1->point,v2->next->point))
            v1->is_ear = IsDiagonal(v0,v3);

          if(IsConvex(v3->prev->point,v3->point,v3->next->point))
            v3->is_ear = IsDiagonal(v1,v4);  

          vertices_to_process--;
          if(vertices_to_process <= 3)
            break;
        }
      }

    }

  }
}