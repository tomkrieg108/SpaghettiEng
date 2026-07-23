# pragma once

#include <vector>
//#include <unordered_map>
#include <string>
#include <cstdint> 
#include <bitset>

 /*
  {}
  []
*/ 

namespace Spg
{

  using MeshId = uint32_t;

  enum class MeshType : uint8_t
  {
    Grid, Coords, Plane, Cube, Sphere, Cylinder, Capsule, Other
  };

  enum class MeshBaseType
  {
    Float, Int, Bool
  };

  enum class MeshAttribute : uint8_t
  {
    Position, Normal, TexCoords, Color, Tangent, Bitangent
  };

  enum class MeshUsage : uint8_t
  {
    Static, Dynamic
  };

  struct MeshElement
  {
    MeshAttribute attribute;
    uint32_t offset_in_bytes = 0;  
  };

  struct MeshLayout
  {
    std::vector<MeshElement> element_list;
    std::bitset<8> attribute_set = 0;
    uint32_t size_in_bytes = 0;

    MeshLayout& PushAttribute(MeshAttribute attribute);
    bool HasAttribute(MeshAttribute attribute);
    uint32_t GetOffsetInBytes(MeshAttribute attribute);
  };  
  
  struct Mesh
  {
    MeshId id;
    MeshUsage usage = MeshUsage::Static;
    std::string name;
    MeshType type;
    MeshLayout layout;
    
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
  };

  // Used in ECS
  struct MeshComponent
  {
    MeshId id;
  };

  //==========================================================
  // Util functions
  //===========================================================
  static uint32_t MeshBaseTypeSizeBytes(MeshBaseType base_type);
  static MeshBaseType GetMeshBaseType(MeshAttribute attribute);

  static uint32_t MeshComponentCount(MeshAttribute attribute);
  static uint32_t MeshElementSizeBytes(MeshAttribute tag);


}
