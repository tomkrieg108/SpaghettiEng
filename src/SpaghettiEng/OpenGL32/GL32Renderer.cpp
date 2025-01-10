#include <glad/gl.h>
#include "GL32Renderer.h"

namespace Spg
{
  namespace Renderer_V1
  {

  
    void GLRenderer::Initialise()
    {
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS); 
      glEnable(GL_LINE_SMOOTH);

      //glEnable(GL_BLEND); 
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //glEnable(GL_CULL_FACE);
      //glCullFace(GL_BACK);
      //glFrontFace(GL_CW);	//or GL_CCW 
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

    void Draw()
    {

    }

    void Begin()
    {

    }
    
    void End()
    {

    }

    uint32_t GLRenderer::GetGLDrawMode(DrawMode draw_mode)
    {
      switch (draw_mode)
      {
      case DrawMode::Triangles: return GL_TRIANGLES;
      case DrawMode::Lines: return GL_LINES;
      }

      //TODO assert unknown draw mode
      return GL_TRIANGLES;
    }

  }

  inline namespace Renderer_V2
  {
      uint32_t GLRenderer::s_instance_count = 0;

      GLRenderer::GLRenderer()
      {
        Initialise();    
      }

      GLRenderer::~GLRenderer()
      {

      }

      void GLRenderer::Initialise()
      {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS); 
        glEnable(GL_LINE_SMOOTH);

        //glEnable(GL_BLEND); 
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CW);	//or GL_CCW       
      }

      Scope<GLRenderer> GLRenderer::Create()
      {
        SPG_ASSERT(GLRenderer::s_instance_count == 0);
        auto renderer = CreateScope<GLRenderer>();
        s_instance_count++;
        return renderer;
      }

      void GLRenderer::Begin()
      {

      }

      void GLRenderer::End()
      {

      }

      void GLRenderer::Draw()
      {

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

}