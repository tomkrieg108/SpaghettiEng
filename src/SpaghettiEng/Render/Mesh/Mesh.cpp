# include "SpaghettiEng/Render/Mesh/Mesh.h"

#include <cstdint> 
#include <bitset>
#include <algorithm>

/*
  {}
  []
*/ 

namespace Spg
{

  MeshLayout& MeshLayout::PushAttribute(MeshAttribute attribute)
  {
    MeshElement el;
    el.attribute = attribute;
    el.offset_in_bytes = this->size_in_bytes;

    element_list.push_back(el);
    attribute_set.set(static_cast<uint8_t>(attribute));
    size_in_bytes += MeshElementSizeBytes(attribute);

    return *this;
  }

  bool MeshLayout::HasAttribute(MeshAttribute attribute)
  {
    return attribute_set.test(static_cast<uint8_t>(attribute));
  }

  uint32_t MeshLayout::GetOffsetInBytes(MeshAttribute attribute)
  {
    if(!HasAttribute(attribute))
      return size_in_bytes;
    for(auto& el : element_list)
    {
      if(el.attribute == attribute)
        return el.offset_in_bytes;
    }
    return size_in_bytes;
  }

  //==================================================================
  // Util functions
  //==================================================================

  uint32_t MeshBaseTypeSizeBytes(MeshBaseType base_type)
  {
    switch(base_type)
      {
        case MeshBaseType::Float: return 4;
        case MeshBaseType::Int: return 4;
        case MeshBaseType::Bool: return 1;
        default: return 0;
      }
      return 0;
  }

  MeshBaseType GetMeshBaseType(MeshAttribute attribute)
  {
    return MeshBaseType::Float; 
  }

  uint32_t MeshComponentCount(MeshAttribute attribute)
  {
    switch (attribute)
      {
        case MeshAttribute::Position:
        case MeshAttribute::Normal:
        case MeshAttribute::Tangent:
        case MeshAttribute::Bitangent:
          return 3;
        case MeshAttribute::TexCoords:
          return 2;
        case MeshAttribute::Color:
          return 4;
        default:
          return 0;    
      }
      return 0;
  }

  uint32_t MeshElementSizeBytes(MeshAttribute attribute)
  {
    auto base_type = GetMeshBaseType(attribute);
    return MeshComponentCount(attribute) * MeshBaseTypeSizeBytes(base_type);
  }

  //==================================================================
  // Mesh layout
  //==================================================================





}