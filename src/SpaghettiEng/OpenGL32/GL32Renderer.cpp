#include <glad/gl.h>
#include "GL32Renderer.h"

namespace Spg
{
  using PrimitiveType = GLRenderer::PrimitiveType;

  static BufferLayout s_layout =
  {
    {"a_position", ShaderDataType::Float3},
    {"a_color", ShaderDataType::Float4}
  };  

  GLRenderer::GLRenderer()
  {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(12.0f); //can be at least 64.0
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);	//or GL_CCW     
    GLShaderBuilder shader_builder;
    m_basic_shader = shader_builder.Add(ShaderType::Vertex, "basic.vs").Add(ShaderType::Fragment, "basic.fs").Build("Basic Shader");
  }

  
  static GLenum GetGLDrawPrimitive(PrimitiveType draw_mode)
  {
    switch (draw_mode) {
      case PrimitiveType::Triangle : return GL_TRIANGLES;
      case PrimitiveType::Line : return GL_LINES;
      case PrimitiveType::LineLoop : return GL_LINE_LOOP;
      case PrimitiveType::LineStrip : return GL_LINE_STRIP;
      case PrimitiveType::Point : return GL_POINTS;
      default: return GL_LINES;
    }
  }

  void GLRenderer::Draw(const GLVertexArray& vertex_array, const GLShader& shader, PrimitiveType draw_mode)
  {
    shader.Bind();
    vertex_array.Bind();
    auto& vbo = vertex_array.GetVertexBuffer();
    auto primitive = GetGLDrawPrimitive(draw_mode);
    glDrawArrays(primitive, 0, vbo.GetVertexCount());
    vertex_array.Unbind();
    shader.Unbind();
  }

  void GLRenderer::Draw(const Camera2D& camera)
  {
    m_basic_shader->Bind();
  
    auto model = glm::mat4(1.0f);
    m_basic_shader->SetUniformMat4f("u_model", model);
    m_basic_shader->SetUniformMat4f("u_proj", camera.GetProjMatrix());
    m_basic_shader->SetUniformMat4f("u_view", camera.GetViewMatrix());
    
    for(auto& element : m_vao_map) {
      Drawable& drawable = element.second;
      if(!drawable.enabled)
        continue;
      drawable.VAO.Bind();
      auto primitive = GetGLDrawPrimitive(drawable.draw_mode);
      auto& vbo = drawable.VAO.GetVertexBuffer();
      glDrawArrays(primitive, 0, vbo.GetVertexCount());
      drawable.VAO.Unbind();
    }
    m_basic_shader->Unbind();
   
  }

  uint32_t GLRenderer::Submit(const std::vector<float>& data, PrimitiveType draw_mode)
  {
    GLVertexArray vao;
    uint32_t size_in_bytes = static_cast<uint32_t>(data.size() * sizeof(float));
    auto vbo = GLVertexBuffer{(void *)data.data(), size_in_bytes, s_layout };   
    vao.AddVertexBuffer(vbo);
    Drawable drawable{vao,draw_mode,true};
    m_vao_map[vao.GetId()] = drawable;
    return vao.GetId();
  }

  uint32_t GLRenderer::Submit(const std::vector<glm::vec2>& position_data, const std::vector<glm::vec3>& colour_data, PrimitiveType draw_mode)
  {
    SPG_ASSERT(position_data.size() == colour_data.size());
    std::vector<float> vertices;
    for(uint32_t i=0; i<position_data.size(); ++i) {
      glm::vec2 pos = position_data[i];
      glm::vec3 col = colour_data[i];
      vertices.insert(std::cend(vertices), { pos.x, pos.y,0.1f,col.x,col.y,col.z,1 });
    }
    return Submit(vertices, draw_mode);
  }

  uint32_t GLRenderer::Submit(const std::vector<glm::vec2>& position_data, glm::vec3 colour, PrimitiveType draw_mode)
  {
    std::vector<float> vertices;
    for(uint32_t i=0; i<position_data.size(); ++i) {
      glm::vec2 pos = position_data[i];
      vertices.insert(std::cend(vertices), { pos.x, pos.y,0.1f,colour.x,colour.y,colour.z,1 });
    }
    return Submit(vertices, draw_mode);
  }

  
  void GLRenderer::UpdatePosition(uint32_t render_id, glm::vec2 position_data, uint32_t index)
  {
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
    GLVertexBuffer vbo = m_vao_map[render_id].VAO.GetVertexBuffer();

    //pos data needs to be a vec3!
    glm::vec3 pos_3d = glm::vec3(position_data,0); 
   
    auto offset_bytes = index * vbo.GetLayout().GetStride() + vbo.GetLayout().GetOffset("a_position");
    vbo.UpdateRawData(offset_bytes, sizeof(glm::vec3), &pos_3d);
  }

  void GLRenderer::UpdateColour(uint32_t render_id, glm::vec3 colour, uint32_t index)
  {
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
    GLVertexBuffer vbo = m_vao_map[render_id].VAO.GetVertexBuffer();
    
    //colour needs to be a vec4
    glm::vec4 col = glm::vec4(colour,1.0f);

    auto offset_bytes = index * vbo.GetLayout().GetStride() + vbo.GetLayout().GetOffset("a_color");
    vbo.UpdateRawData(offset_bytes, sizeof(glm::vec4), &col);
  }

  void GLRenderer::UpdatePosition(uint32_t render_id, const std::vector<glm::vec2>& position_data)
  {
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
    GLVertexBuffer vbo = m_vao_map[render_id].VAO.GetVertexBuffer();

    for(uint32_t index = 0; index < position_data.size(); ++index) {
      //pos data needs to be a vec3!
      glm::vec3 pos_3d = glm::vec3(position_data[index],0); 
      auto offset_bytes = index * vbo.GetLayout().GetStride() + vbo.GetLayout().GetOffset("a_position");
      vbo.UpdateRawData(offset_bytes, sizeof(glm::vec3), &pos_3d);
    }
  }

  void GLRenderer::UpdateColour(uint32_t render_id, const std::vector<glm::vec3>& colour)
  {
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
    GLVertexBuffer vbo = m_vao_map[render_id].VAO.GetVertexBuffer();

    for(uint32_t index = 0; index < colour.size(); ++index) {
      //col data needs to be a vec4!
      glm::vec4 col_4d = glm::vec4(colour[index],1); 
      auto offset_bytes = index * vbo.GetLayout().GetStride() + vbo.GetLayout().GetOffset("a_color");
      vbo.UpdateRawData(offset_bytes, sizeof(glm::vec4), &col_4d);
    }
  }

  void GLRenderer::Update(uint32_t render_id, const std::vector<glm::vec2>& position_data, const std::vector<glm::vec3>& colour_data, const std::vector<uint32_t> indices)
  {
    SPG_ASSERT(position_data.size() == colour_data.size());
    SPG_ASSERT(indices.size() <= position_data.size() );
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
    GLVertexBuffer vbo = m_vao_map[render_id].VAO.GetVertexBuffer();

    std::vector<glm::vec3> pos_3d;
    std::vector<glm::vec4> col_4d;

    for(uint32_t i = 0; i < indices.size(); ++i) {

      uint32_t index = indices[i];
      SPG_ASSERT(index < position_data.size());

      //pos data needs to be a vec3!
      glm::vec3 pos_3d = glm::vec3(position_data[index],0);
      //col data needs to be a vec4!
      glm::vec4 col_4d = glm::vec4(colour_data[index],1); 

      auto offset_bytes = index * vbo.GetLayout().GetStride() + vbo.GetLayout().GetOffset("a_position");
      vbo.UpdateRawData(offset_bytes, sizeof(glm::vec3), &pos_3d);

      offset_bytes = index * vbo.GetLayout().GetStride() + vbo.GetLayout().GetOffset("a_color");
      vbo.UpdateRawData(offset_bytes, sizeof(glm::vec3), &col_4d);
    }
  }

  void GLRenderer::Enable(uint32_t render_id)
  {
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
		auto& drawable =  m_vao_map[render_id];
    drawable.enabled = true;
  }

  void GLRenderer::Disable(uint32_t render_id)
  {
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
		auto& drawable =  m_vao_map[render_id];
    drawable.enabled = false;
  }

  void GLRenderer::Delete(uint32_t render_id)
  {
    SPG_ASSERT (m_vao_map.find(render_id) != m_vao_map.end());
    auto& drawable =  m_vao_map[render_id];
    drawable.VAO.Release();
    m_vao_map.erase(render_id);
  }

 
  void GLRenderer::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height)
  {
    glViewport(x, y, width, height);
  }

  void GLRenderer::SetClearColor(const glm::vec4& color)
  {
    glClearColor(color.r, color.g, color.b, color.a);
  }

  void GLRenderer::ClearBuffers()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

  void GLRenderer::SetLineWidth(float width)
  {
    glLineWidth(width);
  }

}