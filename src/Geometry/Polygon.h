#pragma once

#include <iostream>

#include "CoreLib/Core.h"
#include "Geometry/GeomUtils.h"

namespace Geom
{
  namespace SP
  {
    struct Vertex
    {
      explicit Vertex(const Geom::Point2d& point) : point{point} {}
      Geom::Point2d point;
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
      Polygon(const std::vector<Geom::Point2d>& points);
      std::vector<Geom::Point2d> GetEars();
      std::vector<Vertex*> vertices;
    };
  }

  using PolygonSimple = SP::Polygon;
}