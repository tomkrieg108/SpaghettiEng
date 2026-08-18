#include "SpaghettiEng/Render/Mesh/Mesh.h"

#include <cstdint> 
#include <bitset>
#include <algorithm>

#include "CoreLib/Core.h"
 // {} []

namespace Spg
{

  static uint32_t AttributehBaseTypeBytes(MeshAttributeBaseType base_type);
  static MeshAttributeBaseType GetAttribBaseType(MeshAttributeType attribute_type);
  
  MeshLayout& MeshLayout::PushAttribute(MeshAttributeType attribute_type, 
     MeshAttributeBaseType base_type, 
     uint32_t component_count)
  {
    MeshAttribute attr;
    attr.attribute_type = attribute_type;
    attr.base_type = base_type;
    attr.component_count = component_count;
    attr.offset_in_bytes = size_in_bytes;

    attribute_list.push_back(attr);
    attribute_set.set(static_cast<uint8_t>(attribute_type));
    size_in_bytes += AttributehBaseTypeBytes(base_type)*component_count;

    return *this;
  }

  MeshLayout& MeshLayout::PushAttribute(MeshAttributeType attribute_type)
  {
    return PushAttribute(attribute_type, GetAttribBaseType(attribute_type), 
      AttributeComponentCount(attribute_type) );
  }

  bool MeshLayout::HasAttribute(MeshAttributeType attribute_type)
  {
    return attribute_set.test(static_cast<uint8_t>(attribute_type));
  }

  uint64_t MeshLayout::GetOffsetInBytes(MeshAttributeType attribute_type)
  {
    if(!HasAttribute(attribute_type))
    {
      SPG_ERROR("Layout does not contain attribute type: {}", (uint32_t)attribute_type);
      return size_in_bytes;
    }
      
    for(auto& attr : attribute_list)
    {
      if(attr.attribute_type == attribute_type)
        return attr.offset_in_bytes;
    }
    return size_in_bytes;
  }

  Mesh::Mesh(const std::string& name, MeshPrimitive primitive, MeshUsage usage, MeshTopology topology,  MeshLayout layout):
      name{name}, usage{usage}, topology{topology}, primitive{primitive}, layout{layout}
  {}

  void Mesh::SetVertexBuffer(std::vector<std::byte>&& buffer)
  {
    vertex_buffer = std::move(buffer);
    SPG_ASSERT((vertex_buffer.size() % layout.size_in_bytes) == 0);
    vertex_count = vertex_buffer.size() / layout.size_in_bytes;
  }

  void Mesh::SetData(std::vector<std::byte>&& buffer, std::vector<uint32_t>&& indices_){
    SetVertexBuffer(std::move(buffer));
    indices = std::move(indices_);
    index_count = indices.size();
  }

  //==================================================================
  // Util functions
  //==================================================================

  uint32_t AttributehBaseTypeBytes(MeshAttributeBaseType base_type)
  {
    switch(base_type)
    {
      case MeshAttributeBaseType::Float: return sizeof(float); //4
      case MeshAttributeBaseType::Int: return sizeof(int32_t); //4
      case MeshAttributeBaseType::UInt: return sizeof(uint32_t); //4
      case MeshAttributeBaseType::Bool: return sizeof(bool); //1
    }
    return 0;
  }

  MeshAttributeBaseType GetAttribBaseType(MeshAttributeType attribute)
  {
    //* For normal cases - may differ in special cases
    switch (attribute)
    {
      case MeshAttributeType::BoneIndices:
      case MeshAttributeType::BoneWeights:
        return MeshAttributeBaseType::UInt;
      default:
        return MeshAttributeBaseType::Float;   
    }
    return MeshAttributeBaseType::Float; 
  }

  uint32_t AttributeComponentCount(MeshAttributeType attribute)
  {
    //* For normal cases - may differ in special cases
    switch (attribute)
    {
      case MeshAttributeType::Position:
      case MeshAttributeType::Normal:
      case MeshAttributeType::Tangent:
      case MeshAttributeType::Bitangent:
      case MeshAttributeType::BoneWeights: 
        return 3;
      case MeshAttributeType::TexCoords:
        return 2;
      case MeshAttributeType::Color:
      case MeshAttributeType::BoneIndices:
        return 4;
    }
    return 0;
  }

  
}