#pragma once
#include <glm/glm.hpp>
#include <Common/Common.h> //Scope

namespace Spg
{
  enum class DrawMode
  {
    Triangles, Lines
  };

  //Todo - future - derive this from a base Renderer class
  class GLRenderer
  {
  public:
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
    void Draw();

    static void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
    static void SetClearColor(const glm::vec4& color);
    static void ClearBuffers();
    static void SetLineWidth(float width);

    int val = 42;
  
  private:   
      // Todo - render stats - number of draw calls, uniform uploads, total vertices
    
  };
  
}