#include "MeshPrimitives2D.h"

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
}
