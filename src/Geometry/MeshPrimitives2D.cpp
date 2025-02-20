
#include "MeshPrimitives2D.h"
#include "GeomUtils.h"
#include "ConvexHull.h"
#include <Common.h>
#include <random>
#include <numbers>

namespace Geom
{
  std::vector<float> GenerateGridMesh_XY(float grid_size, float unit_size)
  {
    std::vector<float> vertices;
    const float col = 0.5f; //colour
    const float z = -0.01f;
    
    float y = -grid_size;
    while (y < grid_size + 0.1f)
    {
      if(std::abs(y) < 0.01f)
      {
        vertices.insert(std::cend(vertices), {-grid_size,y,z, 1,0,0,1.0f });
        vertices.insert(std::cend(vertices), { grid_size,y,z, 1,0,0,1.0f });     
      }
      else
      {
        vertices.insert(std::cend(vertices), {-grid_size,y,z, col,col,col,1.0f });
        vertices.insert(std::cend(vertices), { grid_size,y,z, col,col,col,1.0f });
      }
      y += unit_size;
    }

    float x = -grid_size;
    while (x < grid_size + 0.1f)
    {
      if(std::abs(x) < 0.01f)
      {
        vertices.insert(std::cend(vertices), { x,-grid_size,z, 0,1,0,1.0f });
        vertices.insert(std::cend(vertices), { x, grid_size,z, 0,1,0,1.0f });
      }
      else
      {
        vertices.insert(std::cend(vertices), { x,-grid_size,z, col,col,col,1.0f });
        vertices.insert(std::cend(vertices), { x, grid_size,z, col,col,col,1.0f });
      }
      x += unit_size;
    }
    return vertices;
  }

  std::vector<Point2d> GenerateEarClipplingDiagonals(PolygonSimple* polygon)
  {
    std::vector<Point2d> points;
    std::vector<SP::Edge> diagonals; //output arg to Triangulate_EarClipping() 
    Triangulate_EarClipping(polygon, diagonals);
    for(auto d : diagonals) {
      points.push_back(d.v1.point);
      points.push_back(d.v2.point);
    }
    return points;
  }

  std::vector<Point2d> GenerateMonotoneDiagonals(DCEL::Polygon* polygon)
  {
    std::vector<Point2d> points;
    std::vector<LineSeg2D> diagonals;

    auto monoton_polys = GetMonotonPolygons(polygon, diagonals);
    for(auto seg : diagonals) {
      points.push_back(seg.start);
      points.push_back(seg.end);
    }
    return points;
  }

  std::vector<Point2d> GenerateRandomPoints_XY(float radius, uint32_t num_points)
  {
    std::random_device rand_device;
    std::mt19937 gen(rand_device());
    std::uniform_real_distribution<float> dist(-radius,radius);

    std::vector<Point2d> points;
    for(uint32_t i = 0; i < num_points; ++i )
    {
      Point2d point{dist(gen), dist(gen)};
      points.push_back(point);
    }
    return points;
  }

  std::vector<Point2d> GenerateCircle_XY(float radius, uint32_t num_vertices)
  {
    std::vector<Point2d> points;
    constexpr auto pi = std::numbers::pi;
   
    double angle = 0;
    for(uint32_t i = 0; i< num_vertices; ++i)
    {
      angle = (i*pi*2) / num_vertices;
      Point2d p{std::cosf(angle)*radius, std::sinf(angle)*radius};
      points.push_back(p);
    }
    return points;
  }

  // Move a point towards/away from the centroid
  static Point2d perturbPoint(const Point2d& p, const Point2d& centroid, float maxOffset) 
  {
    float dx = centroid.x - p.x;
    float dy = centroid.y - p.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist > 0) {
        float factor = (rand() % 1000 / 1000.0) * maxOffset;
        return { p.x + dx * factor, p.y + dy * factor };
    }
    return p;
  }

  // Move a point towards/away from the centroid dynamically
  static Point2d perturbPoint(const Point2d& p, const Point2d& centroid, double baseOffset, double scaleFactor) {
      double dx = centroid.x - p.x;
      double dy = centroid.y - p.y;
      double dist = std::sqrt(dx * dx + dy * dy);
      
      if (dist > 0) {
          double factor = (rand() % 1000 / 1000.0) * baseOffset * scaleFactor;
          return { p.x + dx * factor, p.y + dy * factor };
      }
      return p;
  }

  std::vector<Point2d> GenerateRandomPolygon_XY(uint32_t num_vertices, float perturb_factor)
  {
    std::vector<Point2d> points = Geom::GenerateRandomPoints_XY(500, num_vertices);
    std::vector<Point2d> hull = Geom::Convexhull2D_ModifiedGrahams(points);
    Point2d centroid = Geom::ComputeCentroid(points);

    //perturb points
    for (auto& p : hull) {
        p = perturbPoint(p, centroid, -perturb_factor); // Move inward
    }
    for (auto& p : points) {
        if (std::find(hull.begin(), hull.end(), p) == hull.end()) {
            p = perturbPoint(p, centroid, perturb_factor); // Move outward
        }
    }

    //sort by polar angle from centroid
    std::sort(points.begin(), points.end(), [&](Point2d a, Point2d b) {
        return atan2(a.y - centroid.y, a.x - centroid.x) < atan2(b.y - centroid.y, b.x - centroid.x);
    });

    return points;
  }

 
 std::vector<Point2d> GenerateRandomPolygon_XY(uint32_t max_points, uint32_t min_points, float perturb_factor, float min_edge, float max_edge, float min_angle)
{
    std::random_device rand_device;
    std::mt19937 gen(rand_device());
    std::uniform_real_distribution<float> dist(0,1);

    std::vector<Point2d> finalPolygon;

    while(finalPolygon.size() < min_points) {
      finalPolygon.clear();
      std::vector<Point2d> points = GenerateRandomPoints_XY(500, max_points);
      std::vector<Point2d> hull = Convexhull2D_ModifiedGrahams(points);
      Point2d centroid = ComputeCentroid(points);

      // Perturb points dynamically
      for (auto& p : hull) {
          float scaleFactor = 0.5f + dist(gen); // Dynamic scaling
          p = perturbPoint(p, centroid, -perturb_factor, scaleFactor);
      }
      for (auto& p : points) {
          if (std::find(hull.begin(), hull.end(), p) == hull.end()) {
              float scaleFactor = 0.5f + dist(gen); // Dynamic scaling
              p = perturbPoint(p, centroid, perturb_factor, scaleFactor);
          }
      }
      // Sort by polar angle from centroid
      std::sort(points.begin(), points.end(), [&](Point2d a, Point2d b) {
          return atan2(a.y - centroid.y, a.x - centroid.x) < atan2(b.y - centroid.y, b.x - centroid.x);
      });

      // Apply length and angle constraints
      //SPG_TRACE("--------------------------------------------");
      finalPolygon.push_back(points[0]);
      for (size_t i = 1; i < points.size(); i++) {
          auto dist = glm::distance(finalPolygon.back(), points[i]);
          if (dist < min_edge || dist > max_edge) {
              continue;
          }
          if (finalPolygon.size() > 1) {
              float angle = ComputeAngleInDegreesChatGPT(finalPolygon[finalPolygon.size() - 2], finalPolygon.back(), points
              [i]);
              angle = 180 - std::abs(angle); // = interior (acute) angle subtended by the 3 consecutive points
              //SPG_TRACE("Angle: {} ", angle);
              if (angle < min_angle) { 
                  continue; //reject if angle too acute
              }
              if( 180 - angle < min_angle)
                continue; //reject if angle too close to 180  

          }
          finalPolygon.push_back(points[i]);
      }
    }
    return finalPolygon;
}

  std::vector<float> GetMeshFromPoints(const std::vector<Point2d>& points, const glm::vec4& colour)
  {
    std::vector<float> vertices;
    for(auto p : points)
    {
      vertices.insert(std::cend(vertices), { p.x, p.y,0, colour.r,colour.g,colour.b,colour.a });
    }
    return vertices;
  }


}
