#pragma once
#include "DCEL.h"
#include "GeomBase.h"
#include <map>

namespace Geom
{
  //todo - Utils?
  static inline bool operator < (const Point2d& p1, const Point2d& p2)
  {
    if(!Geom::Equal(p1.y, p2.y))
        return p1.y < p2.y;
      return p1.x > p2.x;  
  }

  //todo - Utils?
  static inline bool operator > (const Point2d& p1, const Point2d& p2)
  {
    if(!Geom::Equal(p1.y, p2.y))
        return p1.y > p2.y;
      return p1.x < p2.x;  
  }

  class MonotonePartitionAlgo
  {
  public:  

    enum class VertexCategory {
      Start, End, Regular, Split, Merge, Invalid
    }; 

    struct Event
    {
      DCEL::Vertex* vertex = nullptr;
      VertexCategory vertex_category = VertexCategory::Invalid;
      int32_t tag = -1; //For Testing only
    };

    //Event also serves as a 'helper' in the status structure
    using HelperPoint = Event;

    struct EventComparator
    {
      bool operator ()(const Event& e1, const Event& e2) const noexcept
      {
        Point2d p1 = e1.vertex->point;
        Point2d p2 = e2.vertex->point;
        return p1 > p2; //If true, p1 goes before p2.  If false p1 does not go before p2.
      }
    };

    struct EdgeComparator
    {
      EdgeComparator(Point2d& event_point) : event_point{event_point} {}
      float ComputeSweepLineXIntercept(const LineSeg2D& seg) const noexcept
      {
        float y_sweep = event_point.y;
        if(Geom::IsVertical(seg))
          return seg.start.x;  
        if(Geom::IsHorizontal(seg))
          return std::min(seg.start.x,seg.end.x);  
        if(Geom::Equal(seg.start.y, y_sweep))
          return seg.start.x;
        if(Geom::Equal(seg.end.y, y_sweep))
          return seg.end.x;
        
        float x = (seg.start.x - seg.end.x) / (seg.start.y - seg.end.y) *(y_sweep - seg.end.y) + seg.end.x;

        auto seg_min_x = std::min(seg.start.x,seg.end.x);
        auto seg_max_x = std::max(seg.start.x,seg.end.x);  
        SPG_ASSERT((x >= seg_min_x) && (x <= seg_max_x));
        
        return x;
      }
    
      bool operator ()(const DCEL::HalfEdge& e1, const DCEL::HalfEdge& e2) const noexcept
      {
        LineSeg2D seg1 {e1.origin->point, e1.next->origin->point };
        LineSeg2D seg2 {e2.origin->point, e2.next->origin->point };
        float x1 = ComputeSweepLineXIntercept(seg1);
        float x2 = ComputeSweepLineXIntercept(seg2);
        if(!Geom::Equal(x1,x2))
          return x1<x2; //If true, e1 goes before e2.  If false e1 does not go before e2.
        else {
          //todo - Utils?
          auto seg1_min_x = std::min(seg1.start.x,seg1.end.x);
          auto seg2_min_x = std::min(seg2.start.x,seg2.end.x); 
          if(!Geom::Equal(seg1_min_x,seg2_min_x))
            return seg1_min_x < seg2_min_x;
          auto seg1_max_x = std::max(seg1.start.x,seg1.end.x);
          auto seg2_max_x = std::max(seg2.start.x,seg2.end.x);  
          if(!Geom::Equal(seg1_max_x,seg2_max_x))
            return seg1_max_x < seg2_max_x;  

          auto seg1_min_y = std::min(seg1.start.y,seg1.end.y);
          auto seg2_min_y = std::min(seg2.start.y,seg2.end.y); 
          if(!Geom::Equal(seg1_min_y,seg2_min_y))
            return seg1_min_y < seg2_min_y;
          auto seg1_max_y = std::max(seg1.start.y,seg1.end.y);
          auto seg2_max_y = std::max(seg2.start.y,seg2.end.y);  
          if(!Geom::Equal(seg1_max_y,seg2_max_y))
            return seg1_max_y < seg2_max_y;  
          return false;   
        }
      }

      Point2d& event_point;
    };

  public:

    MonotonePartitionAlgo();
    MonotonePartitionAlgo(const std::vector<Point2d>& points);
    void Set(const std::vector<Point2d>& points);
    void Clear();
    void Step();
    bool FinishedProcessing() {
      return m_event_queue.empty();
    }
    void MakeMonotone();
    void Triangulate();
    std::vector<Point2d> GetMonotonDiagonals() {
      return GetDiagonalEndPoints(m_monotone_diagonals);
    }
    std::vector<Point2d> GetTriangulationDiagonals() {
      return GetDiagonalEndPoints(m_triangulation_diagonals); 
    }

    auto& GetDCEL() {
      return m_polygon;
    }
    auto& GetEventPoints() {
      return m_event_queue_unsorted;
    }
    auto& GetEventQueue() {
      return m_event_queue;
    }
    auto& GetStatusStructure() {
      return m_T;
    }

  private:
    using DiagonalList =  std::vector<std::pair<DCEL::Vertex*, DCEL::Vertex*>>;

    VertexCategory GetVertexCategory(DCEL::Vertex* vertex);
    void InitialiseEventQueue();
    void HandleStartVertex(const Event& e);
    void HandleEndVertex(const Event& e);
    void HandleSplitVertex(const Event& e);
    void HandleMergeVertex(const Event& e);
    void HandleRegularVertex(const Event& e);
    DCEL::HalfEdge* GetDepartingEdge(DCEL::Vertex* v); 
    bool PolygonInteriorOnRight(DCEL::Vertex* v);
    void TriangulateFace(DCEL::Face* face);
    std::vector<Point2d> GetDiagonalEndPoints(DiagonalList& diagonal_list);
    
  private:
    void PrintEvent(Event e);
    void PrintStatusStucture();
    void PrintDiagonals();

  private:  
    DCEL m_polygon;
    //use  std::vector rather than priority_queue make debugging easier.  
    std::vector<Event> m_event_queue;  
    //retain the unsorted events (same order as the vertices supplied to DCEL).  Return in GetEventPoints().  Need this for rendering in the Geom App (correct colour of the vertex for given category)
    std::vector<Event> m_event_queue_unsorted; 
    Point2d m_cur_event_point{FLT_MAX,FLT_MAX};
    //The set of active edges for the current algo state ("status structure")
    std::map<DCEL::HalfEdge, HelperPoint, EdgeComparator> m_T;
    //List of diagonals found
    DiagonalList m_monotone_diagonals;
    DiagonalList m_triangulation_diagonals;
  };

}