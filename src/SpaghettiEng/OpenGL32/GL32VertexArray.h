#pragma once

#include "GL32Buffer.h"

namespace Spg
{
  class GLVertexArray
{
  public:
    GLVertexArray();
    ~GLVertexArray();

    void Bind() const;
    void Unbind() const;
    void Release();

    void AddVertexBuffer(const GLVertexBuffer& vertex_buffer);
    void SetIndexBuffer(const GLIndexBuffer& index_buffer);

    const std::vector<GLVertexBuffer>& GetVertexBuffers() const { return m_vertex_buffers; }
    const GLVertexBuffer& GetVertexBuffer(uint32_t index = 0) const { 
      SPG_ASSERT(index < m_vertex_buffers.size());
      return m_vertex_buffers[index]; 
    }
    const GLIndexBuffer& GetIndexBuffer() const { return m_index_buffer; }
    const uint32_t GetId() const { return m_id; }

  private:
    uint32_t m_id = 0;
    uint32_t m_buffer_index = 0;
    std::vector<GLVertexBuffer> m_vertex_buffers; //TODO - need multiple VB's?
    GLIndexBuffer m_index_buffer;
  };
}