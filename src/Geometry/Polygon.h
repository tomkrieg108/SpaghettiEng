#pragma once
#include "GeomBase.h"
#include <Common/Common.h>


namespace Geom
{

  //Simple Polygon representation
  namespace SP
  {
    struct Vertex
    {
      explicit Vertex(const Point2d& point) : point{point} {}
      Point2d point;
      Vertex* next = nullptr;
      Vertex* prev = nullptr;

      //For ear clipping algo
      bool is_ear = false;
      bool is_processed = false;
    };

    struct Edge
    {
      Edge(Vertex _v1, Vertex _v2 ) : v1{_v1}, v2{_v2} {}
      Vertex v1;
      Vertex v2;
    };

    struct Polygon
    {
      Polygon(const std::vector<Point2d>& points);
      std::vector<Point2d> GetEars();
      std::vector<Vertex*> vertices;
    };
  }

  //Doubly Connected Edge List polygon represebtation. Ref video 28
  namespace DCEL
  { 
    /*
      An Edge connects 2 vertices. An edge are paired (twins) in opposite directions. (origin of an edge = destination of it's twin etc) One edge associated with one polygon.  An edge is associated with the polygon (A Face) to it's left.  Edge twinse twins associated with adjacent polygons.  This maintains the CCW ordering convention.
      A Vertex is Corner point at the boundary of polygon. It is also the origin or destination of an Edge
    */

    static uint32_t s_id = 1;

    struct Vertex;
    struct Edge;
    struct Face;

    struct Vertex
    {
      Point2d point;    // Coords of the vertex
      Edge* incident_edge = nullptr; //Incident edge to the vertex. This vertex is origin of of the first Edge created using this vertex. (There coould also be other edges that have this vertex as the origin which is okay)

      explicit Vertex(Point2d point) : point{point} {}
      void Print() { 
        SPG_TRACE("P:([],[])",point.x, point.y);
      }
    };
    
    struct Edge
    {
      Vertex* origin = nullptr;  
      Edge* twin = nullptr;
      Edge* next = nullptr;
      Edge* prev = nullptr;
      Face* incident_face = nullptr;
      int32_t id;
       
      Edge() { id = -1; }
      explicit Edge(Vertex* origin) : origin{origin} { id = s_id++; }

      Vertex* Destination() {
        return twin->origin;
      }

      void Print() {
        SPG_TRACE("E([]): ([],[])->([],[])", id, origin->point.x, origin->point.y, Destination()->point.x, Destination()->point.y);
      }

    };

    struct Face
    {
      //one or the outer boundary edges (only need 1 - can use it to iterate through the others)
      Edge* outer = nullptr;  
      std::vector<Edge*> inner;  //stores 1 edge to each hole in the polygon  

      std::vector<Edge*> GetEdges();

      std::vector<Point2d> GetPoints();

      void Print();
    };

  
    class Polygon
    {
    public:
      //Assume given points are in CCW order
      explicit Polygon(std::vector<Point2d>& points);

      //Insert Edge between vertices v1 and v2
      bool Split(Vertex* v1, Vertex* v2);

      //Join the 2 faces seperated by Edge between v1 and v2
      bool Join(Vertex* v1, Vertex* v2);

      std::vector<Vertex*> GetVertices() {
        return m_vertices;
      }

      std::vector<Edge*> GetEdges() {
        return m_edges;
      }

      std::vector<Face*> GetFaces() {
        return m_faces;
      }

      Vertex* GetVertex(Point2d point);

      void GetEdgesWithSamefaceAndGivenOrigins(Vertex* v1, Vertex* v2, Edge** edge_leaving_v1, Edge** edge_leaving_v2);

    private:
      std::vector<Vertex*> m_vertices;
      std::vector<Edge*> m_edges;
      std::vector<Face*> m_faces;

      Edge* empty_edge = new Edge();

    };



  }

  using PolygonSimple = SP::Polygon;
  using PolygonDCEL = DCEL::Polygon;


}