#include <glad/gl.h> 
#include "GL32Buffer.h"

namespace Spg
{
  GLVertexBuffer::GLVertexBuffer(void* const data, uint32_t size, const BufferLayout& layout) :
	m_layout{ layout }
  {
    glGenBuffers(1,&m_id); //v2.0+
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);  //v2.0
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_vertex_count = size / layout.GetStride();
  }

  GLVertexBuffer::~GLVertexBuffer()
  {
    //CORE_WARN("OpenGLVertexBuffer destructor called: ID={}", m_id);
    //glDeleteBuffers(1, &m_id);
  }

  void GLVertexBuffer::Release()
  {
    glDeleteBuffers(1, &m_id);
    m_id = m_vertex_count = 0;
    m_layout.Reset();
  }

  void GLVertexBuffer::Bind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
  }

  void GLVertexBuffer::Unbind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void GLVertexBuffer::UpdateData(int32_t offset_bytes, uint32_t data_size_bytes, const void* data) const
  {
    SPG_ASSERT(offset_bytes + data_size_bytes <= m_vertex_count*m_layout.GetStride());
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER,offset_bytes,data_size_bytes,data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void GLVertexBuffer::UpdateVertexData(uint32_t vertex_index, uint32_t vertex_size_bytes, const void* data) const
  {
    SPG_ASSERT(vertex_index < m_vertex_count);
    auto stride = m_layout.GetStride();
    SPG_ASSERT(vertex_size_bytes % stride == 0);
    UpdateData(vertex_index*stride, vertex_size_bytes, data);
  }

//---------------------------------------------------

  GLIndexBuffer::GLIndexBuffer(uint32_t* indices, uint32_t count)
  {
    //glCreateBuffers(1, &m_id); //V4.5
    glGenBuffers(1,&m_id); //v2.0+
    //From Hazel:
    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
  }

  GLIndexBuffer::~GLIndexBuffer()
  {
    //CORE_WARN("OpenGLIndexBuffer destructor called: ID={}", m_id);
    //glDeleteBuffers(1, &m_id);
  }

  void GLIndexBuffer::Release()
  {
    glDeleteBuffers(1, &m_id);
    m_id = m_index_count = 0;
  }

  void GLIndexBuffer::Bind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
  }

  void GLIndexBuffer::Unbind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

}