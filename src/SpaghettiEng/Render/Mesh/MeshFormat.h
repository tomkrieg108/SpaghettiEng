#pragma once

#include <cstdint>
#include <vector>

namespace Spg
{
  //default to AOS (interleaved attributes)
  //SOA only for specific scenarios - do later...maybe!

  struct MeshElement
  {
    enum class Tag
    {
      Position, Normal, TexCoords, Color, Tangent, Bitangent, Null
    };

    enum class BaseType
    {
      Float, Int, Bool
    };

    static uint32_t BaseTypeSizeBytes(BaseType base_type)
    {
      switch(base_type)
      {
        case BaseType::Float: return 4;
        case BaseType::Int: return 4;
        case BaseType::Bool: return 1;
        default: return 0;
      }
      return 0;
    }

    static BaseType GetBaseType(Tag tag)
    {
      switch (tag)
      {
        case Tag::Position:
        case Tag::Normal:
        case Tag::Tangent:
        case Tag::Bitangent:
        case Tag::TexCoords:
        case Tag::Color:
          return BaseType::Float;
        default:
          return BaseType::Float;
      }
      return BaseType::Float;
    }  

    static uint32_t ComponentCount(Tag tag)
    {
      switch (tag)
      {
        case Tag::Position:
        case Tag::Normal:
        case Tag::Tangent:
        case Tag::Bitangent:
          return 3;
        case Tag::TexCoords:
          return 2;
        case Tag::Color:
          return 4;
        default:
          return 0;    
      }
      return 0;
    }

    static uint32_t ElementSizeBytes(Tag tag)
    {
      auto base_type = GetBaseType(tag);
      return ComponentCount(tag) * BaseTypeSizeBytes(base_type);
    }

    Tag tag = Tag::Null;
    uint32_t offset = 0;
  };


  struct MeshLayout
  {
    std::vector<MeshElement> element_list;
    uint32_t size = 0;  

    MeshLayout& Push(MeshElement mesh_element)
    {
      element_list.push_back(mesh_element);

      auto& last_element = element_list.back();
      last_element.offset = size;
      size += MeshElement::ElementSizeBytes(last_element.tag);
      return *this;
    }
  };

  enum class MeshLayoutType //Standard layouts
  {
    PN,     //Pos, Normal
    PNT,    //Pos, Normal, TexCoords
    PNC,    //Pos, Normal, Colour
    Other,  //Non-standard
  };

  struct MeshFormat
  {
    MeshLayout mesh_layout;
    MeshLayoutType mesh_layout_type;
    bool is_indexed = false;
  };

}