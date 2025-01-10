#pragma once
#include <glm/glm.hpp>
#include <Common/Common.h> //Scope

namespace Spg
{
  enum class DrawMode
  {
    Triangles, Lines
  };

  namespace Renderer_V1
  {
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
      static void Initialise();
      static void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
      static void SetClearColor(const glm::vec4& color);
      static void ClearBuffers();
      static void SetLineWidth(float width);
     
      // static void Draw(const Mesh& mesh, const Material& material, 
      //   const glm::mat4& transform, Shader& shader);
      
      // static void Begin(const Camera& camera, const DirectionalLight& dir_light,
      //   const glm::mat4& camera_transform, const glm::mat4& light_transform);

      static void Draw();

      static void Begin();
      static void End();

    
    private:
      static uint32_t GetGLDrawMode(DrawMode draw_mode);
    };
  }

  inline namespace Renderer_V2
  {
    //Todo - future - derive this from a base Renderer class
    class GLRenderer
    {
    public:
      GLRenderer();
      ~GLRenderer();

      static Scope<GLRenderer> Create();

      void Begin();
      void End();
      void Draw();

      static void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
      static void SetClearColor(const glm::vec4& color);
      static void ClearBuffers();
      static void SetLineWidth(float width);
    
    private:  
      void Initialise();
      static uint32_t s_instance_count;
       /*
        TODO - render stats - number of draw calls, uniform uploads, total vertices
      */
    };
  }

}