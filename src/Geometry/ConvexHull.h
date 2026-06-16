#pragma once
#include <vector>

#include "MathLib/Geom/Geom.h"

namespace Geom
{
  std::vector<SpgMth::Point2d> ConvexHull2D_GiftWrap(const std::vector<SpgMth::Point2d>& points);

  std::vector<SpgMth::Point2d> Convexhull2D_ModifiedGrahams(const std::vector<SpgMth::Point2d>& points);
  
}