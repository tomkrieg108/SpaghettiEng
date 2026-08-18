#include "SpaghettiEng/Render/Mesh/MeshData.h"

#include <vector>
#include <string>
#include <cstdint>  //uint32_t
#include <numbers>  //std::pi
#include <cstddef> // std::byte
#include <cstring> // memcpy

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

      //todo - crashes!
      //mesh_cache.Add(GenerateSphereMeshTM(), "sphere_tm"); 
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
    
    Mesh mesh{"grid", MeshPrimitive::Coords, MeshUsage::Static, MeshTopology::Lines, layout};
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
    
    Mesh mesh{"plane", MeshPrimitive::Coords, MeshUsage::Static, MeshTopology::Triangles, layout};
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
    
    Mesh mesh{"plane_tm", MeshPrimitive::Coords, MeshUsage::Static, MeshTopology::Triangles, layout};
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
    
    Mesh mesh{"cube", MeshPrimitive::Coords, MeshUsage::Static, MeshTopology::Triangles, layout};
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
    
    Mesh mesh{"cube_tm", MeshPrimitive::Coords, MeshUsage::Static, MeshTopology::Triangles, layout};
    mesh.SetVertexBuffer(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)));
    return mesh;
  }

  Mesh GenerateSphereMeshTM()
  {
    std::vector<float> positions;
    std::vector<float> uv;
    std::vector<float> normals;

    std::vector<float> vertices;
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
      vertices.push_back(positions[i]);
      vertices.push_back(positions[i+1]);
      vertices.push_back(positions[i+2]);
      if (normals.size() > 0)
      {
        vertices.push_back(normals[i]);
        vertices.push_back(normals[i+1]);
        vertices.push_back(normals[i+2]);
      }
      if (uv.size() > 0)
      {
        vertices.push_back(uv[i]);
        vertices.push_back(uv[i+1]);
      }
    }

    MeshLayout layout;
    layout.PushAttribute(MeshAttributeType::Position);
    layout.PushAttribute(MeshAttributeType::Normal);
    layout.PushAttribute(MeshAttributeType::TexCoords);
    
    Mesh mesh{"sphere_tm", MeshPrimitive::Coords, MeshUsage::Static, MeshTopology::Triangles, layout};
    mesh.SetData(ToRawBytes(vertices.data(), vertices.size()*sizeof(float)), std::move(indices));
    return mesh;
  }

}