#include "SpaghettiEng/Render/Mesh/MeshData.h"

#include <vector>
#include <string>
#include <cstdint>  //uint32_t
#include <numbers>  //std::pi
#include <cstddef> // std::byte
#include <cstring> // memcpy

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "CoreLib/Core.h"

#include "SpaghettiEng/Render/Mesh/Mesh.h"
#include "SpaghettiEng/Resource/ResourceCache.h"

/*
  {} []
*/

namespace Spg
{
  static Mesh GenerateCoordsMesh(float size = 5.0f);
  static Mesh GenerateGridMesh(float size = 20.0f);

  static Mesh GeneratePlaneMesh(float size = 20.0f);
  static Mesh GeneratePlaneMeshTM(float size = 20.0f);

  static Mesh GenerateCubeMesh(float size = 0.5f);
  static Mesh GenerateCubeMeshTM(float size = 0.5f);

  static Mesh GenerateSphereMeshTM();
  static Mesh GenerateSphereMesh();

  static std::vector<std::byte> ToRawBytes(const void* source_data, uint32_t source_bytes)
  {
    std::vector<std::byte> raw_data;
    raw_data.resize(source_bytes);
    memcpy(raw_data.data(), source_data, source_bytes);
    return raw_data;  
  }

  //Alternatively
  #if 0
    #include <vector>
    #include <cstddef>
    #include <cstring>
    #include <type_traits>

    template <typename T>
    std::vector<std::byte> ToRawBytes(const std::vector<T>& source) {
        // Safety check: ensure T is safe to copy like raw bytes
        static_assert(std::is_trivially_copyable_v<T>, "Type T must be trivially copyable!");

        const auto* byte_ptr = reinterpret_cast<const std::byte*>(source.data());
        size_t total_bytes = source.size() * sizeof(T);

        // Single-pass allocation and copy
        return std::vector<std::byte>(byte_ptr, byte_ptr + total_bytes);
    }
    // Clean, safe, and zero-copy transfer due to NRVO
    std::vector<std::byte> raw = ToRawBytes(vertices); 

  #endif
    
  #if 0

    struct Vertex {
      float x, y, z;
      float u, v;
    };

std::vector<std::byte> GenerateMeshDirect() {
    // 1. Define your vertex count dynamically
    size_t vertex_count = 100; 
    
    // 2. Safely calculate total byte size
    size_t total_bytes = vertex_count * sizeof(Vertex);
    
    // 3. Allocate and zero out the memory block once
    std::vector<std::byte> raw_bytes(total_bytes);
    
    // 4. Create a typed pointer mapping over the raw memory
    auto* vertex_array = reinterpret_cast<Vertex*>(raw_bytes.data());
    
    // 5. Populate it safely using array syntax
    for (size_t i = 0; i < vertex_count; ++i) {
        vertex_array[i] = Vertex{ 1.0f, 2.0f, 3.0f, 0.0f, 0.0f };
    }
    
    // Zero-copy move out of the function via NRVO
    return raw_bytes; 
}

  #endif

  namespace MeshData
  {
    void Generate(ResourceCache<Mesh>& mesh_cache)
    {
      mesh_cache.Add(GenerateGridMesh(), "grid");
      mesh_cache.Add(GenerateCoordsMesh(), "coords");
      mesh_cache.Add(GeneratePlaneMesh(),"plane");
      mesh_cache.Add(GeneratePlaneMeshTM(),"plane_tm");
      mesh_cache.Add(GenerateCubeMesh(), "cube");
      mesh_cache.Add(GenerateCubeMeshTM(), "cube_tm");
      mesh_cache.Add(GenerateSphereMeshTM(), "sphere_tm"); 
      mesh_cache.Add(GenerateSphereMesh(), "sphere"); 
    }
  }
 
  Mesh GenerateCoordsMesh(float size)
  {
    // Position, Colour (rgba)
    std::vector<float> vertices =
    {
      // positions        // colours (rgba) 
      0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, //x-start
      size, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, //x-end
      
      0.0f, 0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, //y-start
      0.0f, size,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, //y-end

      0.0f, 0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, //z-start
      0.0f, 0.0f,  size,  0.0f, 0.0f, 1.0f, 1.0f, //z-end
    };

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Color);
    
    Mesh mesh{"coords", MeshPrimitive::Coords, MeshUsage::Static, MeshTopology::Lines, layout};
    mesh.SetVertexBuffer(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)));
    return mesh;
  }

  Mesh GenerateGridMesh(float size)
  {
    std::vector<float> vertices;
    const float unit_size = 1.0f;
    const float col = 0.5f; //colour
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

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Color);
    
    Mesh mesh{"grid", MeshPrimitive::Grid, MeshUsage::Static, MeshTopology::Lines, layout};
    mesh.SetVertexBuffer(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)));
    return mesh;
  }

  Mesh GeneratePlaneMesh(float size)
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

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Normal);
    
    Mesh mesh{"plane", MeshPrimitive::Plane, MeshUsage::Static, MeshTopology::Triangles, layout};
    mesh.SetVertexBuffer(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)));
    return mesh;
  }

  Mesh GeneratePlaneMeshTM(float size)
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

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Normal);
    layout.PushAttribute(MeshAttributeType::TexCoords);
    
    Mesh mesh{"plane_tm", MeshPrimitive::Plane, MeshUsage::Static, MeshTopology::Triangles, layout};
    mesh.SetVertexBuffer(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)));
    return mesh;
  }

  Mesh GenerateCubeMesh(float size)
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

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Normal);
    
    Mesh mesh{"cube", MeshPrimitive::Cube, MeshUsage::Static, MeshTopology::Triangles, layout};
    mesh.SetVertexBuffer(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)));
    return mesh;
  }

  Mesh GenerateCubeMeshTM(float size)
  {
    std::vector<float> vertices =
    {
      // positions          // normals    // texture coords
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

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Normal);
    layout.PushAttribute(MeshAttributeType::TexCoords);
    
    Mesh mesh{"cube_tm", MeshPrimitive::Cube, MeshUsage::Static, MeshTopology::Triangles, layout};
    mesh.SetVertexBuffer(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)));
    return mesh;
  }

  Mesh GenerateSphereMeshTM()
  {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    uint32_t index_count = 0;

    constexpr double PI = std::numbers::pi;
    const uint32_t X_SEGMENTS = 64;
    const uint32_t Y_SEGMENTS = 64;

    for (uint32_t x = 0; x <= X_SEGMENTS; ++x)
    {
      for (uint32_t y = 0; y <= Y_SEGMENTS; ++y)
      {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        
        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
      }
    }

    bool oddRow = false;
    for (uint32_t y = 0; y < Y_SEGMENTS; ++y)
    {
      if (!oddRow) // even rows: y == 0, y == 2; and so on
      {
        for (uint32_t x = 0; x <= X_SEGMENTS; ++x)
        {
          indices.push_back(y * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      }

      else
      {
        for (int32_t x = X_SEGMENTS; x >= 0; --x)
        {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back(y * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    index_count = static_cast<uint32_t>(indices.size());

    for (uint32_t i = 0; i < positions.size(); ++i)
    {
      vertices.push_back(positions[i].x);
      vertices.push_back(positions[i].y);
      vertices.push_back(positions[i].z);
      if (normals.size() > 0)
      {
        vertices.push_back(normals[i].x);
        vertices.push_back(normals[i].y);
        vertices.push_back(normals[i].z);
      }
      if (uv.size() > 0)
      {
        vertices.push_back(uv[i].x);
        vertices.push_back(uv[i].y);
      }
    }

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Normal);
    layout.PushAttribute(MeshAttributeType::TexCoords);
    
    Mesh mesh{"sphere_tm", MeshPrimitive::Sphere, MeshUsage::Static, MeshTopology::TriangleStrip, layout};
    mesh.SetData(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)), std::move(indices));
    return mesh;
  }

  Mesh GenerateSphereMesh()
  {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
      for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
      {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
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
    unsigned int indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (normals.size() > 0)
      {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (uv.size() > 0)
      {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Normal);
    layout.PushAttribute(MeshAttributeType::TexCoords);
    
    Mesh mesh{"sphere_tm", MeshPrimitive::Sphere, MeshUsage::Static, MeshTopology::TriangleStrip, layout};
    mesh.SetData(ToRawBytes(data.data(), data.size()*sizeof(float)), std::move(indices));
    return mesh;

  } 

}

/*

//Vertex Math (The Logic)
//Ensure your loops precisely map the grid from pole to pole and seam to seam:

std::vector<float> vertices;
int sectors = 36; // Longitude segments
int stacks = 18;  // Latitude segments
float radius = 1.0f;

float x, y, z, xy;                              // vertex position
float sectorStep = 2 * M_PI / sectors;
float stackStep = M_PI / stacks;
float sectorAngle, stackAngle;

// Loop through stacks (Latitude: +90 to -90 degrees)
for(int i = 0; i <= stacks; ++i) {
    stackAngle = M_PI / 2 - i * stackStep;      // starting from pi/2 to -pi/2
    xy = radius * cosf(stackAngle);             // r * cos(u)
    z = radius * sinf(stackAngle);              // r * sin(u)

    // Loop through sectors (Longitude: 0 to 360 degrees)
    // Note: We use <= sectors (not <) so the seam vertices duplicate positions 
    // but hold unique UV coordinates to prevent texture wrapping glitches.
    for(int j = 0; j <= sectors; ++j) {
        sectorAngle = j * sectorStep;           // starting from 0 to 2*pi

        // Vertex position (x, y, z)
        x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
        y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }
}



*/

//2. Index Generation (The Triangle Assembly)
////A common cause for a "dent" is connecting the wrong vertices at the row boundaries. The grid wrapping must cleanly stitch row i to row i+1:
/*
std::vector<unsigned int> indices;
for(int i = 0; i < stacks; ++i) {
    int k1 = i * (sectors + 1);     // beginning of current stack
    int k2 = k1 + sectors + 1;      // beginning of next stack

    for(int j = 0; j < sectors; ++j, ++k1, ++k2) {
        // 2 triangles per sector grid quad
        // k1 => k2 => k1+1
        if(i != 0) {
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);
        }

        // k1+1 => k2 => k2+1
        if(i != (stacks - 1)) {
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }
}

*/