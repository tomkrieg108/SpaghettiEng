#pragma once
#include <glm/glm.hpp>
#include <Common/Common.h> //Scope
#include "GL32VertexArray.h"
#include "GL32Shader.h"

namespace Spg
{
  //Todo - future - derive this from a base Renderer class
  class GLRenderer
  {
  public:

  enum class DrawMode
  {
    Triangle, Line, LineStrip, LineLoop, Point
  };

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

  public:
    GLRenderer();
    ~GLRenderer() = default;

    void Begin();
    void End();
    void Draw(const GLVertexArray& vertex_array, const GLShader& shader, DrawMode draw_mode);
    void DrawLines(const GLVertexArray& vertex_array, const GLShader& shader);
    void DrawLineStrip(const GLVertexArray& vertex_array, const GLShader& shader);
    void DrawLineLoop(const GLVertexArray& vertex_array, const GLShader& shader);
    void DrawPoints(const GLVertexArray& vertex_array, const GLShader& shader);


    static void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
    static void SetClearColor(const glm::vec4& color);
    static void ClearBuffers();
    static void SetLineWidth(float width);

   
  private:   
      // Todo - render stats - number of draw calls, uniform uploads, total vertices
    
  };
  
}