#pragma once

#include <vector>

#include "MathLib/MathLib.h"
#include "Geometry/Line.h"

#include "Geometry/Polygon.h"
// #include "MathLib/Geom/Line.h"




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

  std::vector<Geom::Point2d> GenerateEarClipplingDiagonals(PolygonSimple* polygon);

  std::vector<Geom::Point2d> GenerateRandomPoints_XY(float radius, uint32_t num_points);

  std::vector<Geom::Point2d> GenerateCircle_XY(float radius, uint32_t num_vertices);

  std::vector<Geom::Point2d> GenerateRandomPolygon_XY(uint32_t num_vertices, float perturb_factor);

  //Generate a random non-convex simple polygon with better control
  std::vector<Geom::Point2d> GenerateRandomPolygon_XY(const PolygonParameters& params);

  std::vector<float> GetMeshFromPoints(const std::vector<Geom::Point2d>& points, const glm::vec4& colour);
}
