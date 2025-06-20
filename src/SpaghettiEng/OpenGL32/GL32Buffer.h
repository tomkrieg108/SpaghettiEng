#pragma once

#include "GL32BufferLayout.h"

namespace Spg
{
  //Todo Support for SOA and AOS

  class GLVertexBuffer
  {
  public:
      GLVertexBuffer() = default;
      GLVertexBuffer(void* const data, uint32_t size, const BufferLayout& layout);
      ~GLVertexBuffer();

      void UpdateRawData(int32_t offset_bytes, uint32_t data_size_bytes, const void* data) const;
      void UpdateVertexData(uint32_t vertex_index, uint32_t vertex_size_bytes, const void* data) const;

      void Bind() const;
      void Unbind() const;
      void Release();

      const BufferLayout& GetLayout() const { return m_layout; }
      uint32_t GetVertexCount() const { return m_vertex_count;  }

    private:
      

    private:  
      uint32_t m_id = 0;
      BufferLayout m_layout;
      uint32_t m_vertex_count = 0;
  };

  //----------------------------------------------

  class GLIndexBuffer
  {
  public:
    GLIndexBuffer() = default;
    GLIndexBuffer(uint32_t* indices, uint32_t count);
    ~GLIndexBuffer();

    void Bind() const;
    void Unbind() const;
    void Release();

    uint32_t GetIndexCount() const { return m_index_count; }

  private:
    uint32_t m_id = 0;
    uint32_t m_index_count = 0;
  };


}