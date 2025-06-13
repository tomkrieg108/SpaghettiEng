#pragma once

#include "GL32VertexArray.h"
#include "GL32Shader.h"
#include <SpaghettiEng/Camera/Camera2D.h>
#include <Common/Common.h> //Scope
#include <glm/glm.hpp>

namespace Spg
{
  //Shader Uniform block definitions
  //TODO - maybe put in shader class or shader utils?
  struct CameraBlock
  {
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 view;
    alignas(16) glm::vec3 eye_pos;
  };

  struct LightBlock
  {
    alignas(16) glm::vec3 dir;
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 ambient_color;
  };  

  class GLRenderer
  {
  public:

    enum class PrimitiveType
    {
      Triangle, Line, LineStrip, LineLoop, Point
    };

    struct Drawable
    {
      GLVertexArray VAO;
      GLRenderer::PrimitiveType draw_mode;
      bool enabled = false;
    }; 

  public:
    GLRenderer();
    ~GLRenderer() = default;

    
    void Draw(const GLVertexArray& vertex_array, const GLShader& shader, PrimitiveType draw_mode);

  
    uint32_t Submit(const std::vector<float>& data, PrimitiveType draw_mode);
    uint32_t Submit(const std::vector<glm::vec2>& position_data, glm::vec3 colour, PrimitiveType draw_mode);
    uint32_t Submit(const std::vector<glm::vec2>& position_data, const std::vector<glm::vec3>& colour_data, PrimitiveType draw_mode);

    void UpdatePosition(uint32_t render_id, glm::vec2 position_data, uint32_t index);
    void UpdateColour(uint32_t render_id, glm::vec3 colour, uint32_t index);

    void UpdatePosition(uint32_t render_id, const std::vector<glm::vec2>& position_data);
    void UpdateColour(uint32_t render_id, const std::vector<glm::vec3>& colour);

    void Update(uint32_t render_id, const std::vector<glm::vec2>& position_data, const std::vector<glm::vec3>& colour_data, const std::vector<uint32_t> indices);

    void Disable(uint32_t render_id);
    void Enable(uint32_t render_id);
    void Delete(uint32_t render_id);

    void Draw(const Camera2D& camera);

    static void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
    static void SetClearColor(const glm::vec4& color);
    static void ClearBuffers();
    static void SetLineWidth(float width);

   
  private:

    //Only this shader will be used
    Scope<GLShader> m_basic_shader = nullptr; //Todo - should be loaded in asset manager and accessed from there

    //Camera2D& m_camera; 
    

    std::unordered_map<uint32_t, Drawable> m_vao_map;

    // Todo - render stats - number of draw calls, uniform uploads, total vertices
    uint32_t m_draw_calls = 0;
    uint32_t m_vertices_rendered = 0;
  };
  
}