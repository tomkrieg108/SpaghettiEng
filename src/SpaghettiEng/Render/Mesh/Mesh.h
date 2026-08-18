# pragma once

#include <vector>
#include <string>
#include <cstdint> 
#include <bitset>
#include <cstddef> // std::byte
#include <cstring> // memcpy

#include "SpaghettiEng/Resource/Resource.h"

 // {} []

namespace Spg
{
  enum class MeshPrimitive
  {
    Grid, Coords, Plane, Cube, Sphere, Cylinder, Capsule, Other
  };

  enum class MeshAttributeType
  {
    Position, Normal, TexCoords, Color, Tangent, Bitangent, BoneIndices, BoneWeights
  };

  enum class MeshAttributeBaseType
  {
    Float, Int, UInt, Bool
  };

  enum class MeshTopology 
  {
    Triangles, Lines, Points
  };

  enum class MeshUsage
  {
    Static, Dynamic
  };

  uint32_t AttributeComponentCount(MeshAttributeType attribute_type); //used in Renderer2.cpp
  
  struct MeshAttribute
  {
    MeshAttributeType attribute_type;
    MeshAttributeBaseType base_type;
    uint32_t component_count;
    uint64_t offset_in_bytes = 0; //64 for conversion to void*
  };

  struct MeshLayout
  {
    std::vector<MeshAttribute> attribute_list;
    std::bitset<8> attribute_set = 0;
    uint64_t size_in_bytes = 0; //64 for conversion to void*

    MeshLayout& PushAttribute(MeshAttributeType attribute_type);
    MeshLayout& PushAttribute(MeshAttributeType attribute_type, 
      MeshAttributeBaseType base_type, 
     uint32_t component_count );
    bool HasAttribute(MeshAttributeType attribute_type);
    uint64_t GetOffsetInBytes(MeshAttributeType attribute_type);
  };  
  
  struct Mesh
  {
    Mesh() = default;

    Mesh(const std::string& name, MeshPrimitive primitive, MeshUsage usage, MeshTopology topology,  MeshLayout layout);

    void SetVertexBuffer(std::vector<std::byte>&& buffer);
    
    void SetData(std::vector<std::byte>&& buffer, std::vector<uint32_t>&& indices_);

    std::string name ="Unnamed mesh";
    MeshPrimitive primitive;
    MeshUsage usage = MeshUsage::Static;
    MeshTopology topology = MeshTopology::Triangles;
    MeshLayout layout;
    
    std::vector<std::byte> vertex_buffer;
    std::vector<uint32_t> indices;

    uint32_t vertex_count = 0;
    uint32_t index_count = 0;
  };

  using MeshID = ResourceID<Mesh>;

  // Used in ECS as a component
  struct MeshHandle
  {
    MeshID mesh_id;
  };

}
