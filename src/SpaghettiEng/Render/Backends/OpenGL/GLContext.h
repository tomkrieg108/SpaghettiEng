#pragma once

struct GLFWwindow;

namespace Spg
{
  //Todo:  Future - defived from a GraphicsContext class
  class GLContext
  {
    public:
      GLContext(GLFWwindow* glfw_window_handle);
      ~GLContext() = default;
      void Initialise();
      void SwapBuffers();
      void MakeContextCurrent();
      
      static void PrintVideoModes(); 
      static void PrintGLParams();
      static void PrintImplInfo();

    private:  
      GLFWwindow* m_glfw_window_handle = nullptr;    
  };

}