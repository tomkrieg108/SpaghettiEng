#pragma once
#include <vector>

#include "Geometry/GeomBase.h"

namespace Geom
{
  std::vector<Geom::Point2d> ConvexHull2D_GiftWrap(const std::vector<Geom::Point2d>& points);

  std::vector<Geom::Point2d> Convexhull2D_ModifiedGrahams(const std::vector<Geom::Point2d>& points);
  
}