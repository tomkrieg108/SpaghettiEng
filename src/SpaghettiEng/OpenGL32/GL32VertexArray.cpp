#include <glad/gl.h> 
#include "GL32VertexArray.h"

namespace Spg
{
  GLVertexArray::GLVertexArray()
  {
    glGenVertexArrays(1, &m_id); //V3.0+
    //glCreateVertexArrays(1, &m_id); //V4.5+
  }

  GLVertexArray::~GLVertexArray()
  {
    //TODO
    // NOTE this gets called when a Mesh local variable is returned from a function, which messing things up
    // use Release() instead - see below

    //CORE_WARN("OpenGLVertexArray destructor called : ID = {}", m_id);
    //glDeleteVertexArrays(1, &m_id);
  }

  void GLVertexArray::Bind() const
  {
    glBindVertexArray(m_id);
  }

  void GLVertexArray::Unbind() const
  {
    glBindVertexArray(0);
  }

  void GLVertexArray::Release()
  {
    glDeleteVertexArrays(1, &m_id);
    m_id = m_buffer_index = 0;

    for (auto& vertex_buffer : m_vertex_buffers)
      vertex_buffer.Release();

    if (m_index_buffer.GetIndexCount() != 0)
      m_index_buffer.Release();
  }

  void GLVertexArray::AddVertexBuffer(const GLVertexBuffer& vertex_buffer)
  {
    //TODO - is there a need to store the vertex buffer- only need the num verticies for the draw call
    glBindVertexArray(m_id);
    vertex_buffer.Bind();
    const auto& layout = vertex_buffer.GetLayout();
    for (const auto& element : layout.GetElements())
    {
      glEnableVertexAttribArray(m_buffer_index);
      if (ShaderUtils::GLBaseTypeIsFloat(element.data_type) )
      {
        glVertexAttribPointer(m_buffer_index, 
          ShaderUtils::GetComponentCount(element.data_type),
          ShaderUtils::ShaderDataTypeToGLBaseType(element.data_type),
          element.normalized ? GL_TRUE : GL_FALSE, 
          layout.GetStride(), 
          (const void*)(element.offset));
      }
      else
      {
        glVertexAttribIPointer(m_buffer_index,
          ShaderUtils::GetComponentCount(element.data_type),
          ShaderUtils::ShaderDataTypeToGLBaseType(element.data_type),
          layout.GetStride(),
          (const void*)(element.offset));
      }
      ++m_buffer_index;
    }
    m_vertex_buffers.push_back(vertex_buffer);
    glBindVertexArray(0);
  }

  void GLVertexArray::SetIndexBuffer(const GLIndexBuffer& index_buffer)
  {
    glBindVertexArray(m_id);
    index_buffer.Bind();
    m_index_buffer = index_buffer;
    glBindVertexArray(0);
  }
}