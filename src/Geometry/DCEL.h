#pragma once
#include "GeomUtils.h"
#include <Common/Common.h>
#include <queue>
#include <vector>
#include <set>
#include <map>

namespace Geom
{
  namespace DCEL_V2 
  {
    class DCEL
    {
    public:
      struct Vertex;
      struct HalfEdge;
      struct Face;

      struct Vertex
      {
        Point2d point;
        HalfEdge* incident_edge = nullptr;
        int32_t tag = -1; //for testing
      };

      struct HalfEdge
      {
        Vertex* origin = nullptr;
        HalfEdge* next = nullptr;
        HalfEdge* prev = nullptr;
        HalfEdge* twin = nullptr;
        Face* incident_face = nullptr; //to the left of this half edge
        int32_t tag = -1; //for testing
      };

      struct Face
      {
        HalfEdge* outer = nullptr; // A half edge for the outer boundary of the face. null if unbounded
        std::vector<HalfEdge*> inner; //A Half edges for each inner boundary (hole) of contained in the face
        int32_t tag = -1; //for testing
      };

      DCEL() = default;
      //For now, assume the input points form a simple polygon coriented CCW
      DCEL(const std::vector<Point2d>& points);

      DCEL(const DCEL& other) = default;

      DCEL& operator = (const DCEL& other) = default;

      DCEL(DCEL&& other) noexcept :
        m_vertices(std::move(other.m_vertices)),
        m_half_edges(std::move(other.m_half_edges)),
        m_faces(std::move(other.m_faces))   
      {
        // Clear the moved-from object
        other.m_vertices.clear();
        other.m_half_edges.clear();
        other.m_faces.clear();
      }
     
      DCEL& operator = (DCEL&& other) noexcept
      {
        if (this != &other) {
          // Free existing resources
          for (auto* v : m_vertices) delete v;
          for (auto* he : m_half_edges) delete he;
          for (auto* f : m_faces) delete f;

          // Move the resources
          m_vertices = std::move(other.m_vertices);
          m_half_edges = std::move(other.m_half_edges);
          m_faces = std::move(other.m_faces);

          // Clear the other object
          other.m_vertices.clear();
          other.m_half_edges.clear();
          other.m_faces.clear();
        }
        return *this;
      }
     
      ~DCEL(); //If implemented, doesn't auto generate move operators.

      void Clear();
      void Set(const std::vector<Point2d>& points);

      struct DiagonalData
      {
        HalfEdge* departing_edge_v1 = nullptr;
        HalfEdge* departing_edge_v2 = nullptr;
        bool is_valid = false;
      };

      DiagonalData DiagonalCheck(Vertex* v1, Vertex* v2);
      void Split(Vertex* v1, Vertex* v2);
      void Validate();
      
      Vertex* GetVertex(int32_t tag); 
      std::vector<HalfEdge*> GetDepartingEdges(Vertex* v);
      auto FindDepartingEdgesWithCommonFace(Vertex* v1, Vertex* v2) -> std::pair<HalfEdge*,HalfEdge*>;
      bool AnyIntersectionsExist(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);
      bool IsConvex(Vertex* v, HalfEdge* departing_edge);
      bool MakesInteriorConnection(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);

      HalfEdge* GetDepartingEdge(Vertex* v, Face* f); 
      bool IsInteriorOnRight(Vertex* v, Face* f);

      //Helpers
      Point2d GetOriginPoint(HalfEdge* e) {
        return e->origin->point;
      }
      Point2d GetDestinationPoint(HalfEdge* e) {
        return e->twin->origin->point;
      }
      LineSeg2D GetLineSeg2d(HalfEdge* e) {
         return LineSeg2D{GetOriginPoint(e),GetDestinationPoint(e)};
      }
      LineSeg2D GetLineSeg2d(Vertex* v_start, Vertex* v_end) {
        return LineSeg2D{v_start->point, v_end->point};
      }
      const auto& GetVertices() const {
        return m_vertices;
      }

      //logging
      void PrintVertices();
      void PrintHalfEdges();
      void PrintFace(Face* f);
      void PrintFaces();
    
    private:  
      std::vector<Vertex*> m_vertices;
      std::vector<HalfEdge*> m_half_edges;
      std::vector<Face*> m_faces;
    };
  
    static_assert(std::is_copy_constructible<DCEL>::value);
    static_assert(std::is_move_constructible<DCEL>::value);
  }

  /*********************************************************************************************
    Monotone Partitioning
  *********************************************************************************************/
  namespace Monotone_V2
  {
    using DCEL_vertex = Geom::DCEL_V2::DCEL::Vertex;
    using DCEL_HalfEdge = Geom::DCEL_V2::DCEL::HalfEdge;
    using DCEL_Face = Geom::DCEL_V2::DCEL::Face;

    enum class VertexCategory {
      Start, End, Regular, Split, Merge, Invalid
    }; 

    //todo - Utils?
    inline bool operator < (const Point2d& p1, const Point2d& p2)
    {
      if(!Geom::Equal(p1.y, p2.y))
          return p1.y < p2.y;
        return p1.x > p2.x;  
    }

    //todo - Utils?
    inline bool operator > (const Point2d& p1, const Point2d& p2)
    {
      if(!Geom::Equal(p1.y, p2.y))
          return p1.y > p2.y;
        return p1.x < p2.x;  
    }
    
    struct Event
    {
      DCEL_vertex* vertex = nullptr;
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
    
      bool operator ()(const DCEL_HalfEdge& e1, const DCEL_HalfEdge& e2) const noexcept
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

   
    /*
      Class To partition a polygon into monoton pieces. 'MonotonePartitioner' might be another reasonable name for this
    */
    class MonotoneSpawner5000
    {
    public:

      MonotoneSpawner5000();

      MonotoneSpawner5000(const std::vector<Point2d>& points);

      void Set(const std::vector<Point2d>& points);
      void Clear();

      void Step();

      bool FinishedProcessing() {
        return m_event_queue.empty();
      }
      
      void MakeMonotone();

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

      std::vector<Point2d> GetDiagonalEndPoints();
     
    private:
      VertexCategory GetVertexCategory(DCEL_vertex* vertex);
      void InitialiseEventQueue();

      void HandleStartVertex(const Event& e);
      void HandleEndVertex(const Event& e);
      void HandleSplitVertex(const Event& e);
      void HandleMergeVertex(const Event& e);
      void HandleRegularVertex(const Event& e);

      //Helpers
      DCEL_HalfEdge* GetDepartingEdge(DCEL_vertex* v); 
      bool PolygonInteriorOnRight(DCEL_vertex* v);
      
    private:
      void PrintEvent(Event e);
      void PrintStatusStucture();
      void PrintDiagonals();

    private:  

      DCEL_V2::DCEL m_polygon;

      //std::priority_queue<Event, std::vector<Event>, EventComparator> m_event_queue; //Todo - look into std::heap, std::make_heap
      //use  std::vector to make debugging easier
      std::vector<Event> m_event_queue; 
      std::vector<Event> m_event_queue_unsorted; 
      Point2d m_cur_event_point{FLT_MAX,FLT_MAX};

      //The set of active edges for the current algo state ("status structure")
      std::map<DCEL_HalfEdge, HelperPoint, EdgeComparator> m_T;

      //List of diagonals found
      std::vector<std::pair<DCEL_vertex*, DCEL_vertex*>> m_diagonals;
    };

    class MonotoneTriangulator9000 //Thats totally a word
    {
    public:
      
    private:
    };

  }
    
  
}