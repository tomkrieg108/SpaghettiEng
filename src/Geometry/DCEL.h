#pragma once
#include "GeomUtils.h"
#include <Common/Common.h>
//#include <queue>
// #include <vector>
// #include <set>
// #include <map>

namespace Geom
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
      std::vector<HalfEdge*> inner; //A Half edges for each inner boundary (hole) contained in the face
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
    bool Validate() const;

    //Bunch of Helperfunctions
    Vertex* GetVertex(int32_t tag); 
    std::vector<HalfEdge*> GetDepartingEdges(Vertex* v);
    auto FindDepartingEdgesWithCommonFace(Vertex* v1, Vertex* v2) -> std::pair<HalfEdge*,HalfEdge*>;
    bool AnyIntersectionsExist(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);
    bool IsConvex(Vertex* v, HalfEdge* departing_edge);
    bool MakesInteriorConnection(Vertex* orig, Vertex* dest, HalfEdge* orig_depart_edge);
    HalfEdge* GetDepartingEdge(Vertex* v, Face* f); 
  
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
  
  private:  
    std::vector<Vertex*> m_vertices;
    std::vector<HalfEdge*> m_half_edges;
    std::vector<Face*> m_faces;
  };

  static_assert(std::is_copy_constructible<DCEL>::value);
  static_assert(std::is_move_constructible<DCEL>::value);

}