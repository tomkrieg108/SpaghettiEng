#pragma once
#include "GeomBase.h"

namespace Geom
{
  std::vector<Point2d> ConvexHull2D_GiftWrap(const std::vector<Point2d>& points);

  std::vector<Point2d> Convexhull2D_ModifiedGrahams(const std::vector<Point2d>& points);
  
}