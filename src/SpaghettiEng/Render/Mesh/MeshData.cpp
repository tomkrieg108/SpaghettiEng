# include "SpaghettiEng/Render/Mesh/MeshData.h"

#include <vector>
#include <numbers>  //std::pi
#include <cstdint>  //uint32_t


/*
  {}
  []
*/

namespace Spg
{
  namespace MeshData
  {
    std::vector<float> GenerateCoordsMeshData(float size = 1.0f)
    {
      // Position, Colour (rgba)
      std::vector<float> vertices =
      {
        // positions        // colours (rgba) 
        0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, //x-start
        size, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, //x-start
        
        0.0f, 0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, //y-start
        0.0f, size,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, //y-start

        0.0f, 0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, //z-start
        0.0f, 0.0f,  size,  0.0f, 1.0f, 0.0f, 1.0f, //z-start
        
      };
      return vertices;
    }

    std::vector<float> GenerateGridMeshData(float size = 20.0f)
    {
      std::vector<float> vertices;
      const float unit_size = 1.0f;
      const float col = size; //colour
      const float y = 0.01f; // Raise slightly
      float x, z;

      z = -size;
      while (z < size + 0.1f)
      {
        x = -size;
        vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
        x = +size;
        vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
        z += unit_size;
      }

      x = -size;
      while (x < size + +0.1f)
      {
        z = -size;
        vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
        z = +size;
        vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
        x += unit_size;
      }

      return vertices;
    }

    
    std::vector<float> GeneratePlaneMeshData(float size = 20.0f)
    {
      std::vector<float> vertices =
      {
        // positions          // normals         
        size, -size,  size,   0.0f, 1.0f, 0.0f,  
        -size, -size,  size,  0.0f, 1.0f, 0.0f,   
        -size, -size, -size,  0.0f, 1.0f, 0.0f,   

        size, -size,  size,   0.0f, 1.0f, 0.0f,  
        -size, -size, -size,  0.0f, 1.0f, 0.0f,   
        size, -size, -size,   0.0f, 1.0f, 0.0f,
      };

      return vertices;
    }

    std::vector<float> GeneratePlaneMeshDataTM(float size = 20.0f)
    {
      std::vector<float> vertices =
      {
        // positions          // normals       // texcoords
        size, -size,  size,   0.0f, 1.0f, 0.0f,  size,  0.0f,
        -size, -size,  size,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
        -size, -size, -size,  0.0f, 1.0f, 0.0f,  0.0f, size,

        size, -size,  size,   0.0f, 1.0f, 0.0f,  size,  0.0f,
        -size, -size, -size,  0.0f, 1.0f, 0.0f,  0.0f, size,
        size, -size, -size,   0.0f, 1.0f, 0.0f,  size, size
      };

      return vertices;
    }

    std::vector<float> GenerateCubeMeshData(float size = 0.5f)
    {
      // position (x,y,z), normals (x,y,z)
      std::vector<float> vertices = 
      {
        // position (x,y,z),    normals (x,y,z)
        -size, -size, -size,  0.0f,  0.0f, -1.0f,
        size, -size, -size,  0.0f,  0.0f, -1.0f,
        size,  size, -size,  0.0f,  0.0f, -1.0f,
        size,  size, -size,  0.0f,  0.0f, -1.0f,
        -size,  size, -size,  0.0f,  0.0f, -1.0f,
        -size, -size, -size,  0.0f,  0.0f, -1.0f,

        -size, -size,  size,  0.0f,  0.0f,  1.0f,
        size, -size,  size,  0.0f,  0.0f,  1.0f,
        size,  size,  size,  0.0f,  0.0f,  1.0f,
        size,  size,  size,  0.0f,  0.0f,  1.0f,
        -size,  size,  size,  0.0f,  0.0f,  1.0f,
        -size, -size,  size,  0.0f,  0.0f,  1.0f,

        -size,  size,  size, -1.0f,  0.0f,  0.0f,
        -size,  size, -size, -1.0f,  0.0f,  0.0f,
        -size, -size, -size, -1.0f,  0.0f,  0.0f,
        -size, -size, -size, -1.0f,  0.0f,  0.0f,
        -size, -size,  size, -1.0f,  0.0f,  0.0f,
        -size,  size,  size, -1.0f,  0.0f,  0.0f,

        size,  size,  size,  1.0f,  0.0f,  0.0f,
        size,  size, -size,  1.0f,  0.0f,  0.0f,
        size, -size, -size,  1.0f,  0.0f,  0.0f,
        size, -size, -size,  1.0f,  0.0f,  0.0f,
        size, -size,  size,  1.0f,  0.0f,  0.0f,
        size,  size,  size,  1.0f,  0.0f,  0.0f,

        -size, -size, -size,  0.0f, -1.0f,  0.0f,
        size, -size, -size,  0.0f, -1.0f,  0.0f,
        size, -size,  size,  0.0f, -1.0f,  0.0f,
        size, -size,  size,  0.0f, -1.0f,  0.0f,
        -size, -size,  size,  0.0f, -1.0f,  0.0f,
        -size, -size, -size,  0.0f, -1.0f,  0.0f,

        -size,  size, -size,  0.0f,  1.0f,  0.0f,
        size,  size, -size,  0.0f,  1.0f,  0.0f,
        size,  size,  size,  0.0f,  1.0f,  0.0f,
        size,  size,  size,  0.0f,  1.0f,  0.0f,
        -size,  size,  size,  0.0f,  1.0f,  0.0f,
        -size,  size, -size,  0.0f,  1.0f,  0.0f
      };

      return vertices;
    }

    std::vector<float> GenerateCubeMeshDataTM(float size = 0.5f)
    {
      std::vector<float> vertices =
      {
        // positions          // normals           // texture coords
        -size, -size, -size,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        size, -size, -size,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        size,  size, -size,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        size,  size, -size,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -size,  size, -size,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -size, -size, -size,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -size, -size,  size,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        size, -size,  size,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        size,  size,  size,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        size,  size,  size,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -size,  size,  size,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -size, -size,  size,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -size,  size,  size, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -size,  size, -size, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -size, -size, -size, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -size, -size, -size, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -size, -size,  size, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -size,  size,  size, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        size,  size,  size,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        size,  size, -size,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        size, -size, -size,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        size, -size, -size,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        size, -size,  size,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        size,  size,  size,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -size, -size, -size,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        size, -size, -size,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        size, -size,  size,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        size, -size,  size,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -size, -size,  size,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -size, -size, -size,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -size,  size, -size,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        size,  size, -size,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        size,  size,  size,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        size,  size,  size,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -size,  size,  size,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -size,  size, -size,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
      };

      return vertices;
    }


    std::vector<float> GenerateSphereMeshData()
    {
      std::vector<float> indices;
     
      return indices;
    }

    std::vector<float> GenerateSphereMeshDataTM()
    {
      std::vector<float> positions;
      std::vector<float> uv;
      std::vector<float> normals;
      std::vector<float> data;
      std::vector<uint32_t> indices;
      uint32_t index_count = 0;

      constexpr double PI = std::numbers::pi;
      const uint32_t X_SEGMENTS = 64;
      const uint32_t Y_SEGMENTS = 64;

      for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
      {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
          float xSegment = (float)x / (float)X_SEGMENTS;
          float ySegment = (float)y / (float)Y_SEGMENTS;
          float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
          float yPos = std::cos(ySegment * PI);
          float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

          positions.insert(std::cend(positions),{xPos, yPos, zPos});
          uv.insert(std::cend(uv),{xSegment, ySegment});
          normals.insert(std::cend(normals),{xPos, yPos, zPos});
        }
      }

      bool oddRow = false;
      for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
      {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
          for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
          {
            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          }
        }
        else
        {
          for (int x = X_SEGMENTS; x >= 0; --x)
          {
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x);
          }
        }
        oddRow = !oddRow;
      }
      index_count = static_cast<uint32_t>(indices.size());

      for (unsigned int i = 0; i < positions.size(); ++i)
      {
        data.push_back(positions[i]);
        data.push_back(positions[i+1]);
        data.push_back(positions[i+2]);
        if (normals.size() > 0)
        {
          data.push_back(normals[i]);
          data.push_back(normals[i+1]);
          data.push_back(normals[i+2]);
        }
        if (uv.size() > 0)
        {
          data.push_back(uv[i]);
          data.push_back(uv[i+1]);
        }
      }

      return data;
    }

    
  }

}