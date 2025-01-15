#include <glad/gl.h>
#include "GL32Renderer.h"

namespace Spg
{
  GLRenderer::GLRenderer()
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