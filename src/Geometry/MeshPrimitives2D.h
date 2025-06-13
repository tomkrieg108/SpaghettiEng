#pragma once

#include "GeomBase.h"
#include "Line.h"
#include "Polygon.h"
#include "Triangulate.h"
#include "MonotonePartition.h"
#include "DCEL.h"
#include <vector>

namespace Geom
{
  struct PolygonParameters
  {
    uint32_t min_points;
    uint32_t max_points;
    float min_edge;
    float max_edge;
    float min_angle;
    float perturb_factor;
  };

  std::vector<float> GenerateGridMesh_XY(float grid_size, float unit_size);

  std::vector<Point2d> GenerateEarClipplingDiagonals(PolygonSimple* polygon);

  //std::vector<Point2d> GenerateMonotoneDiagonals(DCEL::Polygon* polygon);
 
  std::vector<Point2d> GenerateRandomPoints_XY(float radius, uint32_t num_points);

  std::vector<Point2d> GenerateCircle_XY(float radius, uint32_t num_vertices);

  std::vector<Point2d> GenerateRandomPolygon_XY(uint32_t num_vertices, float perturb_factor);

  //Generate a random non-convex simple polygon with better control
  std::vector<Point2d> GenerateRandomPolygon_XY(const PolygonParameters& params);

  std::vector<float> GetMeshFromPoints(const std::vector<Point2d>& points, const glm::vec4& colour);
}
