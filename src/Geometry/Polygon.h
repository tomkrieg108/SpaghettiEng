#pragma once
#include "GeomBase.h"
#include <Common/Common.h>
#include <iostream>

namespace Geom
{
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


using PolygonSimple = SP::Polygon;
}