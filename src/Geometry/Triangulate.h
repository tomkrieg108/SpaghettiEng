#pragma once
#include "Polygon.h"

namespace Geom
{

  bool IsDiagonal(const SP::Vertex* v1, const SP::Vertex* v2, const PolygonSimple* polygon = nullptr );

  void Triangulate_EarClipping(PolygonSimple* polygon, std::vector<SP::Edge>& edge_list);

  
}