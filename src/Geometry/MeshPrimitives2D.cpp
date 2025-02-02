
#include "MeshPrimitives2D.h"
#include "GeomUtils.h"
#include "ConvexHull.h"
#include <Common.h>
#include <random>
#include <numbers>

namespace Geom
{
  std::vector<float> CreateGrid_XY(float grid_size, float unit_size)
  {
    std::vector<float> vertices;
    const float col = 0.5f; //colour
    const float z = 0.0f;
    
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

  std::vector<Point2d> CreateCirclePoints_XY(float radius, uint32_t num_vertices)
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

  std::vector<float> CreateCircle_XY(float radius, uint32_t num_vertices)
  {
    std::vector<Point2d> points = CreateCirclePoints_XY(radius, num_vertices);
    std::vector<float> vertices;
    for(auto p : points)
    {
      vertices.insert(std::cend(vertices), { p.x, p.y,0, 0,0,1,1.0f });
    }
    return vertices;
  }

  std::vector<float> CreateRandPolygon_XY(uint32_t num_vertices)
  {
    std::vector<Point2d> circle = CreateCirclePoints_XY(300, num_vertices);

    std::vector<float> vertices;
    std::random_device rand_device;
    std::mt19937 gen(rand_device());

    const float spread = 150.0f;

    std::uniform_real_distribution<float> dist(-spread,spread);
    
    uint32_t i = 0;
    for(auto p : circle)
    {
      glm::vec4 col(0,0,1,1);
      if(i % 2 == 0) 
      {
        col.r = 1;
        col.b = 0;
      }

      float x = p.x + dist(gen);
      float y = p.y + dist(gen); 

      vertices.insert(std::cend(vertices), { x, y ,0 , col.r,col.g, col.b, 1 });

      ++i;
    }
    

    return vertices;
  }

  std::vector<Point2d> CreateRandomPoints_XY(float radius, uint32_t num_points)
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

  std::vector<float> CreateRandomPointMesh_XY(float radius, uint32_t num_points)
  {
    std::vector<Point2d> points = CreateRandomPoints_XY(radius, num_points);
    std::vector<float> vertices;
    for(auto p : points)
    {
      vertices.insert(std::cend(vertices), { p.x, p.y,0, 0,0,1,1.0f });
    }
    return vertices;
  }

  std::vector<float> GetMeshFromPoints(const std::vector<Point2d>& points)
  {
    std::vector<float> vertices;
    for(auto p : points)
    {
      vertices.insert(std::cend(vertices), { p.x, p.y,0, 0,1,1,1 });
    }
    return vertices;
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

  std::vector<Point2d> GenerateRandomPolygon_XY(uint32_t num_vertices, float perturb_factor)
  {
    std::vector<Point2d> points = CreateRandomPoints_XY(500, num_vertices);
    std::vector<Point2d> hull = Geom::Convexhull2D_ModifiedGrahams(points);
    Point2d centroid = Geom::ComputeCentroid2d(points);

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


}
