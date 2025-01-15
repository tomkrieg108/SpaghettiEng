#include "MeshPrimitives2D.h"

namespace Geom
{
  std::vector<float> CreateGrid_XY(float grid_size, float unit_size)
  {
    std::vector<float> vertices;
    const float col = 0.5f; //colour
    const float y = -0.01f;
    float x, z;

    z = -grid_size;
    while (z < grid_size + 0.1f)
    {
      x = -grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      x = +grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      z += unit_size;
    }

    x = -grid_size;
    while (x < grid_size + +0.1f)
    {
      z = -grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      z = +grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      x += unit_size;
    }

    return vertices;
  }
}
