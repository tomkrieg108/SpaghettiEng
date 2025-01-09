#pragma once

#include "GL32Shader.h"
#include "GL32ShaderUtils.h"

namespace Spg
{

  struct BufferElement
  {
    BufferElement(const std::string& attribute_name, ShaderDataType data_type, bool normalized = false) :
      attribute_name{ attribute_name }, data_type{ data_type }, normalized{ normalized }
    {
      size = ShaderUtils::ShaderDataTypeSize(data_type);
    } 

    std::string attribute_name{ "" };
    ShaderDataType data_type{ ShaderDataType::None};
    uint32_t size{ 0 };	//bytes
    bool normalized{ false };
    size_t offset{ 0 };
  };

  class BufferLayout
  {
  public:
    BufferLayout() = default;
    ~BufferLayout() = default;

    BufferLayout(std::initializer_list<BufferElement> element_list) :
      m_element_list{ element_list }
    {
      for (BufferElement& element : m_element_list)
        UpdateStrideAndOffset(element);
    }

    BufferLayout& Add(BufferElement&& element)
    {
      m_element_list.push_back(element);
      UpdateStrideAndOffset(element);
      return *this;
    }

    uint32_t GetStride() const { return m_stride; }

    const std::vector<BufferElement>& GetElements() const { return m_element_list; }

    void Reset() { m_element_list.clear(); }

  private:
    std::vector<BufferElement> m_element_list;
    uint32_t m_stride = 0;

  private:
    void UpdateStrideAndOffset(BufferElement& element)
    {
      element.offset = m_stride;
      m_stride += element.size;
    }
  };

}