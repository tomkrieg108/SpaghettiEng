#pragma once

#include <iostream>

#include "CoreLib/Core.h"
#include "MathLib/Geom/Geom.h"

namespace Geom
{
  namespace SP
  {
    struct Vertex
    {
      explicit Vertex(const SpgMth::Point2d& point) : point{point} {}
      SpgMth::Point2d point;
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
      Polygon(const std::vector<SpgMth::Point2d>& points);
      std::vector<SpgMth::Point2d> GetEars();
      std::vector<Vertex*> vertices;
    };
  }

  using PolygonSimple = SP::Polygon;
}