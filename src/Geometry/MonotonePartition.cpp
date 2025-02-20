
#include "MonotonePartition.h"
#include "GeomBase.h"
#include "GeomUtils.h"
#include <set>
#include <map>

namespace Geom
{
  
  //Vid 33
  VertexCategory CategorizeVertex(DCEL::Vertex* vertex)
  {
    DCEL::Vertex* v_prev= vertex->incident_edge->prev->origin;
    DCEL::Vertex* v_next= vertex->incident_edge->next->origin;

    if( (v_prev == nullptr) || (v_next == nullptr))
      return VertexCategory::Invalid;

    Point2d p_prev = v_prev->point;
    Point2d p = vertex->point;
    Point2d p_next = v_next->point;  

    bool is_left = Left({p_prev,p},p_next);

    if((p.y > p_prev.y) && (p.y > p_next.y)) {
      if(is_left)
        return VertexCategory::Start;
      else
        return VertexCategory::Split;  
    }
    else if ((p.y < p_prev.y) && (p.y < p_next.y)) {
      if(is_left)
        return VertexCategory::End;
      else
        return VertexCategory::Merge;  
    }
    else {
      return VertexCategory::Regular;
    }
  }

  struct DCELVertexWrapper
  {
    DCEL::Vertex* vert;
    VertexCategory category;
  };

  struct DCELVertexWrapperSort
  {
    bool operator () (DCELVertexWrapper& current, DCELVertexWrapper& ref) {
      auto cur_p = current.vert->point;
      auto ref_p = ref.vert->point;
      return( (cur_p.y > ref_p.y) || 
              (Equal(cur_p.y,ref_p.y) && ((cur_p.x < ref_p.x))) );
    }
  };

  struct DCELEdgeWrapper
  {
    DCELEdgeWrapper(DCEL::Edge* _edge, DCELVertexWrapper& _helper) : edge{_edge}, helper{_helper}
    {
      origin = edge->origin->point;
      dest = edge->twin->origin->point;
    }

    //return x-coord of intersection point with the edge and supplied point (Vid 33 ~ 9:00)
    float ConputeX(Point2d point) const 
    {
      float denom = dest.y - origin.y;
      float x = point.x;
      if(!Equal(denom, 0)) {
        x = origin.x + (point.y - origin.y)*(dest.x-origin.x) / denom;
      }
      return x;
    }

    public:
      DCEL::Edge* edge;
      DCELVertexWrapper helper;

    private:
      Point2d origin, dest;
  };

  struct SweepLineComparator
  {
    Point2d* point;
    SweepLineComparator(Point2d* _point) : point{_point} {}

    bool operator()(const DCELEdgeWrapper* ref1, const DCELEdgeWrapper* ref2) const 
    {
      return ref1->ConputeX(*point) < ref2->ConputeX(*point);
    }
  };

  // TODO : Check the posibility of Refactoring the code to remove duplicate lines

  
  /*
  For each of these Handle function, added in an extra output parameter diag_list, which gets added to when a diagonal is found and Split is called
  */
  static void HandleStartVertex(DCELVertexWrapper& vertex
    , std::set<DCELEdgeWrapper*, SweepLineComparator>& sweep_line
    , std::map<DCEL::Edge*, DCELEdgeWrapper*>& edge_mapper, DCEL::Polygon* poly,
     std::vector<LineSeg2D>& diagonals)
  {
    DCELEdgeWrapper* edge = new DCELEdgeWrapper(vertex.vert->incident_edge, vertex);
    sweep_line.insert(edge);
    edge_mapper.insert(std::pair<DCEL::Edge*, DCELEdgeWrapper*>(vertex.vert->incident_edge, edge));
  }

  static void HandleEndVertex(DCELVertexWrapper& vertex
    , std::set<DCELEdgeWrapper*, SweepLineComparator>& sweep_line
    , std::map<DCEL::Edge*, DCELEdgeWrapper*>& edge_mapper, DCEL::Polygon* poly,
    std::vector<LineSeg2D>& diagonals)
  {
    auto edge_wrapper = edge_mapper[vertex.vert->incident_edge->prev];
    auto found = sweep_line.find(edge_wrapper);
    auto helper = (*found)->helper;
    if (helper.category == VertexCategory::Merge) {
      diagonals.push_back({vertex.vert->point, helper.vert->point});
      poly->Split(vertex.vert, helper.vert);
    }
    sweep_line.erase(found);
  }

  static void HandleSplitVertex(DCELVertexWrapper& vertex
    , std::set<DCELEdgeWrapper*, SweepLineComparator>& sweep_line
    , std::map<DCEL::Edge*, DCELEdgeWrapper*>& edge_mapper, DCEL::Polygon* poly, 
     std::vector<LineSeg2D>& diagonals)
  {
    DCELEdgeWrapper* edge = new DCELEdgeWrapper(vertex.vert->incident_edge, vertex);
    auto found = sweep_line.lower_bound(edge);
    DCELEdgeWrapper* ej;
    if (found == sweep_line.end()) {
      if (sweep_line.size() > 0) {
        ej = *(--found);
        diagonals.push_back({vertex.vert->point, ej->helper.vert->point});
        poly->Split(vertex.vert, ej->helper.vert);
        ej->helper = vertex;
      }
    }
    else if (found != sweep_line.begin())
    {
      ej = *(--found);
      diagonals.push_back({vertex.vert->point, ej->helper.vert->point});
      poly->Split(vertex.vert, ej->helper.vert);
      ej->helper = vertex;
    }
    sweep_line.insert(edge);
    edge_mapper.insert(std::pair<DCEL::Edge*, DCELEdgeWrapper*>(vertex.vert->incident_edge, edge));
  }

  static void HandleMergeVertex(DCELVertexWrapper& vertex
    , std::set<DCELEdgeWrapper*, SweepLineComparator>& sweep_line
    , std::map<DCEL::Edge*, DCELEdgeWrapper*>& edge_mapper, DCEL::Polygon* poly,
    std::vector<LineSeg2D>& diagonals)
  {
    auto edge_wrapper = edge_mapper[vertex.vert->incident_edge->prev];
    if (edge_wrapper->helper.category == VertexCategory::Merge) {
      diagonals.push_back({vertex.vert->point, edge_wrapper->helper.vert->point});
      poly->Split(vertex.vert, edge_wrapper->helper.vert);
    }

    auto found = sweep_line.find(edge_wrapper);
    if (found != sweep_line.end())
      sweep_line.erase(found);

    DCELEdgeWrapper* edge = new DCELEdgeWrapper(vertex.vert->incident_edge, vertex);
    found = sweep_line.lower_bound(edge);
    DCELEdgeWrapper* ej;
    if (found == sweep_line.end()) {
      if (sweep_line.size() > 0) {
        ej = *(--found);
        if (ej->helper.category == VertexCategory::Merge) {
          diagonals.push_back({vertex.vert->point, ej->helper.vert->point});
          poly->Split(vertex.vert, ej->helper.vert);
        }
        ej->helper = vertex;
      }
    }
    else if (found != sweep_line.begin())
    {
      ej = *(--found);
      if (ej->helper.category == VertexCategory::Merge) {
        diagonals.push_back({vertex.vert->point, ej->helper.vert->point});
        poly->Split(vertex.vert, ej->helper.vert);   
      }
      ej->helper = vertex;
    }
  }

  static void HandleRegularVertex(DCELVertexWrapper& vertex
    , std::set<DCELEdgeWrapper*, SweepLineComparator>& sweep_line
    , std::map<DCEL::Edge*, DCELEdgeWrapper*>& edge_mapper, DCEL::Polygon* poly,
     std::vector<LineSeg2D>& diagonals)
  {
    // Check whether the interior of the polygon lies right to vertex point
    auto prev_y = vertex.vert->incident_edge->prev->origin->point.y;
    auto current_y = vertex.vert->point.y;
    auto next_y = vertex.vert->incident_edge->next->origin->point.y;

    DCELEdgeWrapper* edge = new DCELEdgeWrapper(vertex.vert->incident_edge, vertex);

    if (prev_y >= current_y && current_y >= next_y) {
      auto edge_wrapper = edge_mapper[vertex.vert->incident_edge->prev];
      if (edge_wrapper->helper.category == VertexCategory::Merge) {
        diagonals.push_back({vertex.vert->point, edge_wrapper->helper.vert->point});
        poly->Split(vertex.vert, edge_wrapper->helper.vert);
      }

      auto found = sweep_line.find(edge_wrapper);
      if (found != sweep_line.end())
        sweep_line.erase(found);

      sweep_line.insert(edge);
      edge_mapper.insert(std::pair<DCEL::Edge*, DCELEdgeWrapper*>(vertex.vert->incident_edge,
        edge));
    }
    else {
      auto found = sweep_line.lower_bound(edge);
      DCELEdgeWrapper* ej;
      if (found == sweep_line.end()) {
        if (sweep_line.size() > 0) {
          ej = *(--found);
          if (ej->helper.category == VertexCategory::Merge) {
            diagonals.push_back({vertex.vert->point, ej->helper.vert->point});
            poly->Split(vertex.vert, ej->helper.vert);
          }
          ej->helper = vertex;
        }
      }
      else if (found != sweep_line.begin())
      {
        ej = *(found--);
        if (ej->helper.category == VertexCategory::Merge) {
          diagonals.push_back({vertex.vert->point, ej->helper.vert->point});
          poly->Split(vertex.vert, ej->helper.vert);
        }
        ej->helper = vertex;
      }
    }
  }

  std::vector<DCEL::Polygon*> GetMonotonPolygons(DCEL::Polygon* poly, std::vector<LineSeg2D>& diagonals)
  {
    std::vector<DCELVertexWrapper> vertices;  //The event queue
    for(auto vertex : poly->GetVertices()) {
      vertices.push_back(DCELVertexWrapper{vertex, CategorizeVertex(vertex)});
    }
    std::sort(vertices.begin(), vertices.end(), DCELVertexWrapperSort());

    Point2d* sweep_point = new Point2d();
    sweep_point->x = vertices[0].vert->point.x;
    sweep_point->y = vertices[0].vert->point.y;

    SweepLineComparator comp(sweep_point);
    std::set<DCELEdgeWrapper*, SweepLineComparator> sweep_line(comp); //sweep line status structure
    std::map<DCEL::Edge*, DCELEdgeWrapper*> edge_mapping;

    //process event queue
    for(auto vertex : vertices) {
      sweep_point->x = vertex.vert->point.x;
      sweep_point->y = vertex.vert->point.y;  

      switch(vertex.category) {
        case VertexCategory::Start:
			    HandleStartVertex(vertex, sweep_line, edge_mapping, poly,diagonals);
			    break;
        case VertexCategory::End:
          HandleEndVertex(vertex, sweep_line, edge_mapping, poly,diagonals);
          break;
        case VertexCategory::Regular:
          HandleRegularVertex(vertex, sweep_line, edge_mapping, poly, diagonals);
          break;
        case VertexCategory::Split:
          HandleSplitVertex(vertex, sweep_line, edge_mapping, poly,diagonals);
          break;
        case VertexCategory::Merge:
          HandleMergeVertex(vertex, sweep_line, edge_mapping, poly,diagonals);
          break;
        case VertexCategory::Invalid:
          break;
      }
    }

    std::vector<std::vector<Point2d>> polygon_pieces_vertices;

    for (auto face_ptr : poly->GetFaces()) {
      auto first_edge_ptr = face_ptr->outer;
      if (first_edge_ptr) {
        std::vector<Point2d> vertices;
        vertices.push_back(first_edge_ptr->origin->point);

        auto next_edge_ptr = first_edge_ptr->next;
        while (next_edge_ptr != first_edge_ptr) {
          vertices.push_back(next_edge_ptr->origin->point);
          next_edge_ptr = next_edge_ptr->next;
        }
        polygon_pieces_vertices.push_back(vertices);
      }
    }

    std::vector<DCEL::Polygon*> monotone_polys;
    for(auto vertices : polygon_pieces_vertices)
		  monotone_polys.push_back(new DCEL::Polygon(vertices));

    return monotone_polys;
  }


}