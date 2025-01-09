#pragma once

struct GLFWwindow;

namespace Spg
{
  class GLContext
  {
    public:
      static void Initialise(GLFWwindow* glfw_window_handle);
      static void SetViewport(int32_t top, int32_t left, int32_t width, int32_t height);
      static void PrintGLParams();
      static void PrintVendorInfo();

    private:  
  };

}