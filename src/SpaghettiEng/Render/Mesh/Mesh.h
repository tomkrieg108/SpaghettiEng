# pragma once

#include <vector>
#include <string>
#include <cstdint> 
#include <bitset>

#include "SpaghettiEng/Resource/Resource.h"

 // {} []

namespace Spg
{

  enum class MeshPrimitive : uint8_t
  {
    Grid, Coords, Plane, Cube, Sphere, Cylinder, Capsule, Other
  };

  enum class MeshAttribute : uint8_t
  {
    Position, Normal, TexCoords, Color, Tangent, Bitangent
  };

  enum class MeshAttributeBaseType
  {
    Float, Int, Bool
  };

  enum class MeshUsage : uint8_t
  {
    Static, Dynamic
  };

  struct MeshElement
  {
    MeshAttribute attribute;
    uint64_t offset_in_bytes = 0; //64 for conversion to void*
  };

  struct MeshLayout
  {
    std::vector<MeshElement> element_list;
    std::bitset<8> attribute_set = 0;
    uint64_t size_in_bytes = 0; //64 for conversion to void*

    MeshLayout& PushAttribute(MeshAttribute attribute);
    bool HasAttribute(MeshAttribute attribute);
    uint64_t GetOffsetInBytes(MeshAttribute attribute);
  };  
  
  struct Mesh
  {
    std::string name ="Unnamed mesh";
    MeshUsage usage = MeshUsage::Static;
    MeshPrimitive primitive;
    MeshLayout layout;
    
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    uint32_t vertex_count = 0;
    uint32_t index_count = 0;
  };

  using MeshID = ResourceID<Mesh>;

  // Used in ECS
  struct MeshHandle
  {
    //MeshHandle(MeshID mesh_id) : mesh_id{mesh_id} {}
    MeshID mesh_id;
  };

  //==========================================================
  // Util functions
  //===========================================================
  static uint32_t AttributehBaseTypeSizeBytes(MeshAttributeBaseType base_type);
  static MeshAttributeBaseType GetAttribBaseType(MeshAttribute attribute);

  uint32_t AttributeComponentCount(MeshAttribute attribute);
  uint32_t AttributeSizeBytes(MeshAttribute attribute);
  
}
