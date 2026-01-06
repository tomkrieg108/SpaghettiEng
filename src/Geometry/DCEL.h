#pragma once
#include "GeomUtils.h"
#include <Common/Common.h>

namespace Geom
{
  inline namespace dcel_v1 
  {

    // TODO: make tags (ID's) incicies into the containers
    class DCEL
    {
    public:
      struct Vertex;
      struct HalfEdge;
      struct Face;

      struct Vertex
      {
        Point2d point; // Setup as origin of the incident edge (by convention)
        HalfEdge* incident_edge = nullptr;
        //for testing
        int32_t tag = -1; 
        static int32_t next_tag;
      };

      struct HalfEdge
      {
        Vertex* origin = nullptr;
        HalfEdge* next = nullptr;
        HalfEdge* prev = nullptr;
        HalfEdge* twin = nullptr;
        Face* incident_face = nullptr; //to the left of this half edge
        //for testing
        static int32_t next_tag;
        int32_t tag = -1; 
      };

      struct Face
      {
        HalfEdge* outer = nullptr; // A half edge for the outer boundary of the face. null if unbounded
        std::vector<HalfEdge*> inner; //A Half edges for each inner boundary (hole) contained in the face
        //for testing
        static int32_t next_tag;
        int32_t tag = -1; //for testing
      };

      struct Diagonal
      {
        HalfEdge* departing_edge_v1 = nullptr;
        HalfEdge* departing_edge_v2 = nullptr;
        bool is_valid = false; //Todo - std::optional generally preferred
      };

      DCEL() = default;
      //For now, assume the input points form a simple polygon oriented CCW
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
      void Init(const std::vector<Point2d>& points);
      std::vector<HalfEdge*> InsertBoundingBox(BoundingBox& bb); //new
     
      std::pair<HalfEdge*,HalfEdge*> MakeHalfEdgePair(); 
      Vertex* MakeVertex(Point2d point);
      Face* MakeFace();
      
      void Connect(Point2d const& p, std::vector<HalfEdge*> he_list); //new
      void Connect(HalfEdge*h1, HalfEdge*h2); //new
      void Connect(Vertex* v, HalfEdge* h); // new
      Vertex* Split(Point2d const& p, HalfEdge* h);  //new

      Diagonal GetDiagonal(Vertex* v1, Vertex* v2);
      void Join(Vertex* v1, Vertex* v2);
      bool Validate() const;

      //Bunch of Helperfunctions
      Vertex* GetVertex(int32_t tag); 
      std::vector<HalfEdge*> GetDepartingEdges(Vertex* v);
      auto FindDepartingEdgesWithCommonFace(Vertex* v1, Vertex* v2) -> std::pair<HalfEdge*,HalfEdge*>;
      bool AnyIntersectionsExist(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);
      bool IsConvex(Vertex* v, HalfEdge* departing_edge);
      bool MakesInteriorConnection(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);
      HalfEdge* GetDepartingEdge(Vertex* v, Face* f); 

      std::optional<std::vector<HalfEdge*>> GetEdgeLoop(HalfEdge* h); //new (Not used)
      std::optional<std::vector<Vertex*>> GetEdgeLoopVertices(HalfEdge* h); //new (Not used)

      Point2d GetOriginPoint(HalfEdge* e) const {
        return e->origin->point;
      }
      Point2d GetDestinationPoint(HalfEdge* e) const {
        return e->twin->origin->point;
      }
      LineSeg2D GetLineSeg2d(HalfEdge* e) const {
          return LineSeg2D{GetOriginPoint(e),GetDestinationPoint(e)};
      }
      LineSeg2D GetLineSeg2d(Vertex* v_start, Vertex* v_end) const {
        return LineSeg2D{v_start->point, v_end->point};
      }
      auto& GetVertices() {
        return m_vertices;
      }
      auto& GetHalfEdges() {
        return m_half_edges;
      }
      auto& GetFaces() {
        return m_faces;
      }

      //Returns the vertices defining the input face
      std::vector<DCEL::Vertex*> GetVertices(DCEL::Face* face);

      //logging
      void PrintVertices();
      void PrintHalfEdges();
      void PrintFace(Face* f);
      void PrintFaces();

      static void Test();
    
    private:  
      std::vector<Vertex*> m_vertices;
      std::vector<HalfEdge*> m_half_edges;
      std::vector<Face*> m_faces;
    };

    static_assert(std::is_copy_constructible<DCEL>::value);
    static_assert(std::is_move_constructible<DCEL>::value);
  }

  namespace dcel_v2
  {
     
    class DCEL
    {
    public:
      struct Vertex;
      struct HalfEdge;
      struct Face;

      struct Vertex
      {
        Point2d* point; // Setup as origin of the incident edge (by convention)
        HalfEdge* incident_edge = nullptr;
        //for testing
        int32_t tag = -1; 
        static int32_t next_tag;
      };

      struct HalfEdge
      {
        Vertex* origin = nullptr;
        HalfEdge* next = nullptr;
        HalfEdge* prev = nullptr;
        HalfEdge* twin = nullptr;
        Face* incident_face = nullptr; //to the left of this half edge
        //for testing
        static int32_t next_tag;
        int32_t tag = -1; 
      };

      struct Face
      {
        HalfEdge* outer = nullptr; // A half edge for the outer boundary of the face. null if unbounded
        std::vector<HalfEdge*> inner; //A Half edges for each inner boundary (hole) contained in the face
        //for testing
        static int32_t next_tag;
        int32_t tag = -1; //for testing
      };

      struct Diagonal
      {
        HalfEdge* departing_edge_v1 = nullptr;
        HalfEdge* departing_edge_v2 = nullptr;
         bool is_valid = false; //Todo - std::optional generally preferred
      };

      DCEL() = default;

      // Assume the input points form a simple polygon oriented CCW
      DCEL(const std::vector<Point2d>& points);

      DCEL (DCEL const& other) = delete;
      DCEL& operator = (DCEL const& other) = delete;

      DCEL(DCEL&& other) = default;
      DCEL& operator = (DCEL&& other) = default;

      std::unique_ptr<DCEL> Clone() const; 

      std::pair<HalfEdge*,HalfEdge*> MakeHalfEdgePair(); 
      Vertex* MakeVertex(Point2d const& point);
      Face* MakeFace();

      void InsertPointLoop(const std::vector<Point2d>& points);
     
      void Init(const std::vector<Point2d>& points);
      void InsertBoundingBox(BoundingBox& bb);
     
      void Connect(Vertex* v, std::vector<HalfEdge*> he_list);
      void Connect(HalfEdge*e1, HalfEdge*e2);

      Diagonal GetDiagonal(Vertex* v1, Vertex* v2);
      void Join(Vertex* v1, Vertex* v2);
      bool Validate() const;

      //Bunch of Helperfunctions
      Vertex* GetVertex(int32_t tag); 
      std::vector<HalfEdge*> GetDepartingEdges(Vertex* v);
      auto FindDepartingEdgesWithCommonFace(Vertex* v1, Vertex* v2) -> std::pair<HalfEdge*,HalfEdge*>;
      bool AnyIntersectionsExist(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);
      bool IsConvex(Vertex* v, HalfEdge* departing_edge);
      bool MakesInteriorConnection(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);
      HalfEdge* GetDepartingEdge(Vertex* v, Face* f); 
      std::optional<std::vector<HalfEdge*>> GetEdgeLoop(HalfEdge* h);
      std::optional<std::vector<Vertex*>> GetEdgeLoopVertices(HalfEdge* h);

      Point2d GetOriginPoint(HalfEdge* e) const {
        return *(e->origin->point);
      }
      Point2d GetDestinationPoint(HalfEdge* e) const {
        return *(e->twin->origin->point);
      }
      LineSeg2D GetLineSeg2d(HalfEdge* e) const {
          return LineSeg2D{GetOriginPoint(e),GetDestinationPoint(e)};
      }
      LineSeg2D GetLineSeg2d(Vertex* v_start, Vertex* v_end) const {
        return LineSeg2D{*v_start->point, *v_end->point};
      }
      auto& GetVertices() {
        return m_vertices;
      }
      auto& GetHalfEdges() {
        return m_half_edges;
      }
      auto& GetFaces() {
        return m_faces;
      }

      //Returns the vertices defining the input face
      std::vector<DCEL::Vertex*> GetVertices(DCEL::Face* face);

      //logging
      void PrintVertices();
      void PrintHalfEdges();
      void PrintFace(Face* f);
      void PrintFaces();

      static void Test();
    
    private:
      std::vector<std::unique_ptr<Point2d>> m_points;
      std::vector<std::unique_ptr<Vertex>> m_vertices;
      std::vector<std::unique_ptr<HalfEdge>> m_half_edges;
      std::vector<std::unique_ptr<Face>> m_faces;
    };

    //static_assert(std::is_copy_constructible<DCEL>::value);
    static_assert(std::is_move_constructible<DCEL>::value);

  }
}