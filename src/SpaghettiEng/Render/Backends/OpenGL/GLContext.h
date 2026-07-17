#pragma once



struct GLFWwindow;

namespace Spg
{
  //Todo:  Future - derived from a base GraphicsContext class
  class OpenGLContext
  {
    public:

      OpenGLContext(GLFWwindow* glfw_window_handle);
      ~OpenGLContext() = default;
      
      void SwapBuffers();
      void ClearBuffer();
      void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
      void PrintSpecs();
      
    private:
  
      void PrintVideoModes(); 
      void PrintGLParams();
      void PrintImplInfo();

    private:
      GLFWwindow* m_glfw_window_handle = nullptr;    
  };

  //Why a graphics context
  /*
    Yes, there is a massive benefit. In a modern OpenGL, GLFW, and GLAD setup, a custom graphics context abstraction acts as a wrapper that hides repetitive, boilerplate setup code and manages your application's state.By default, OpenGL operates as a giant, global state machine. If you do not abstract it, your initialization code quickly becomes cluttered, and managing multiple windows or threads becomes incredibly difficult.

    The Benefits of AbstractionEncapsulation: 

    It hides messy initialization steps (GLFW windows, GLAD function pointer loading) inside a single object.

    State Management: It tracks window dimensions, aspect ratios, and input states without using global variables.

    Life-cycle Control: It automates cleanup (glfwTerminate, destroying windows) using object destruction (RAII in C++).

    Multi-Window Ready: It makes managing multiple windows easy, as each window gets its own isolated context object.

  */
  // #include <glad/glad.h>
  // #include <GLFW/glfw3.h>
  // #include <string>
  namespace Graphics_CTX_V2 //AI
  {
    class GraphicsContext {
    public:
        GraphicsContext(int width, int height, const std::string& title);
        ~GraphicsContext();

        // Prevent copying to avoid accidental window destruction
        GraphicsContext(const GraphicsContext&) = delete;
        GraphicsContext& operator=(const GraphicsContext&) = delete;

        void MakeCurrent();
        void SwapBuffers();
        void PollEvents();
        bool ShouldClose() const;

        GLFWwindow* GetNativeWindow() const { return m_Window; }

    private:
        void Init();

        int m_Width;
        int m_Height;
        std::string m_Title;
        GLFWwindow* m_Window = nullptr;
    };
  }

}