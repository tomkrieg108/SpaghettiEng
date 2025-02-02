#pragma once

#include "GeomBase.h"
#include "Line.h"
#include <vector>

namespace Geom
{
  std::vector<float> CreateGrid_XY(float grid_size, float unit_size);

  std::vector<float> CreateRandPolygon_XY(uint32_t num_vertices);

  std::vector<Point2d> CreateCirclePoints_XY(float radius, uint32_t num_vertices);

  std::vector<float> CreateCircle_XY(float radius, uint32_t num_vertices);

  std::vector<Point2d> CreateRandomPoints_XY(float radius, uint32_t num_points);

  std::vector<float> CreateRandomPointMesh_XY(float radius, uint32_t num_points);

  std::vector<float> GetMeshFromPoints(const std::vector<Point2d>& points);

  std::vector<Point2d> GenerateRandomPolygon_XY(uint32_t num_vertices, float perturb_factor);
}
