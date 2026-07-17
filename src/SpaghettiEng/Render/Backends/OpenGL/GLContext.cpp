
#include "GLContext.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h> 

#include "CoreLib/Core.h"

namespace Spg
{

  static GLenum params[] = { GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, GL_MAX_CUBE_MAP_TEXTURE_SIZE, GL_MAX_DRAW_BUFFERS, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, GL_MAX_TEXTURE_IMAGE_UNITS, GL_MAX_TEXTURE_SIZE, GL_MAX_VARYING_FLOATS, GL_MAX_VERTEX_ATTRIBS, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, GL_MAX_VERTEX_UNIFORM_COMPONENTS, GL_MAX_UNIFORM_BUFFER_BINDINGS, GL_MAX_VIEWPORT_DIMS, GL_STEREO, };

  static const char* param_name[] = { "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", "GL_MAX_CUBE_MAP_TEXTURE_SIZE", "GL_MAX_DRAW_BUFFERS", "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
  "GL_MAX_TEXTURE_IMAGE_UNITS", "GL_MAX_TEXTURE_SIZE", "GL_MAX_VARYING_FLOATS", "GL_MAX_VERTEX_ATTRIBS", "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", "GL_MAX_VERTEX_UNIFORM_COMPONENTS", "GL_MAX_UNIFORM_BUFFER_BINDINGS", "GL_MAX_UNIFORM_BUFFERS" "GL_MAX_VIEWPORT_DIMS", "GL_STEREO" };

  //https://learnopengl.com/In-Practice/Debugging
  static void APIENTRY GLErrorCallback(GLenum source,GLenum type,unsigned int id,GLenum severity,GLsizei length,const char* message,const void* userParam);

  OpenGLContext::OpenGLContext(GLFWwindow* glfw_window_handle) :
    m_glfw_window_handle{glfw_window_handle}
  {
    SPG_ASSERT(m_glfw_window_handle != nullptr);

    glfwMakeContextCurrent(m_glfw_window_handle);
    int status = gladLoadGL(glfwGetProcAddress);
		if (!status)
		{
      SPG_CRITICAL("OpenGL (Glad) Initialisation failed. Terminating");
			glfwDestroyWindow(m_glfw_window_handle);
			glfwTerminate();
		}
    else
      SPG_INFO("OpenGL context Created");

#ifdef SPG_DEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLErrorCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif
  }

  void OpenGLContext::SwapBuffers()
  {
    glfwSwapBuffers(m_glfw_window_handle);
  }

  void OpenGLContext::ClearBuffer()
  {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

  void OpenGLContext::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height)
  {
    glViewport(x, y, width, height);
  }

  void OpenGLContext::PrintSpecs()
  {
    PrintVideoModes();
    PrintImplInfo();
    PrintGLParams();
  }


  
  void OpenGLContext::PrintVideoModes()
  {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    int count;
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
    //std::cout << "Video modes supported: " << count << "\n\n";
    SPG_INFO("Video Modes Supported:");
    for (int i = 0; i < count; i++)
    {
        SPG_TRACE("   {}: {}: {}: {}: {}: {}: {}:", i, modes[i].height, modes[i].width, modes[i].redBits, modes[i].blueBits, modes[i].greenBits, modes[i].refreshRate);
    }
  }

  void OpenGLContext::PrintImplInfo()
  {
    SPG_INFO("OpenGL Implementation Info:");
    SPG_TRACE("  Vendor: {}", reinterpret_cast<char const*>(glGetString(GL_VENDOR)));
    SPG_TRACE("  Renderer: {}", reinterpret_cast<char const*>(glGetString(GL_RENDERER)));
    SPG_TRACE("  Version: {}", reinterpret_cast<char const*>(glGetString(GL_VERSION)));
  }

  void OpenGLContext::PrintGLParams()
  {
    SPG_INFO("OPENGL PARAMETER VALUES");
    for (int i = 0; i <= 11; i++)
    {
        static int param_val;
        glGetIntegerv(params[i], &param_val);
        SPG_TRACE("   {} \t {}", param_name[i], param_val);
    }
    static GLboolean bool_val;
    glGetBooleanv(GL_STEREO, &bool_val);
    SPG_TRACE("   {} \t {}", param_name[11], bool_val);
  }

  //https://learnopengl.com/In-Practice/Debugging
  static void APIENTRY GLErrorCallback(GLenum source,GLenum type,unsigned int id,GLenum severity,GLsizei length,const char* message,const void* userParam)
  {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) 
      return;

    SPG_WARN("-----------------------------------------------------");
    SPG_WARN("GL Error message.  ID: {}.  Description: {}", id, message);

    const char* source_msg = "";
    const char* type_msg = "";
    const char* severity_msg = "";

    switch (source)
    {
      case GL_DEBUG_SOURCE_API:             source_msg = "Source: API"; break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_msg = "Source: Window System"; break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER: source_msg = "Source: Shader Compiler"; break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:     source_msg = "Source: Third Party"; break;
      case GL_DEBUG_SOURCE_APPLICATION:     source_msg = "Source: Application"; break;
      case GL_DEBUG_SOURCE_OTHER:           source_msg = "Source: Other"; break;
      default:															source_msg = "Source: Unknown"; break;
    }

    switch (type)
    {
      case GL_DEBUG_TYPE_ERROR:               type_msg = "Type: Error"; break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_msg = "Type: Deprecated Behaviour"; break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_msg = "Type: Undefined Behaviour"; break;
      case GL_DEBUG_TYPE_PORTABILITY:         type_msg = "Type: Portability"; break;
      case GL_DEBUG_TYPE_PERFORMANCE:         type_msg = "Type: Performance"; break;
      case GL_DEBUG_TYPE_MARKER:              type_msg = "Type: Marker"; break;
      case GL_DEBUG_TYPE_PUSH_GROUP:          type_msg = "Type: Push Group"; break;
      case GL_DEBUG_TYPE_POP_GROUP:           type_msg = "Type: Pop Group"; break;
      case GL_DEBUG_TYPE_OTHER:               type_msg = "Type: Other"; break;
      default:																type_msg = "Type: Unknown"; break;
    }

    switch (severity)
    {
      case GL_DEBUG_SEVERITY_HIGH:					severity_msg = "Severity: high"; break;
      case GL_DEBUG_SEVERITY_MEDIUM:				severity_msg = "Severity: medium"; break;
      case GL_DEBUG_SEVERITY_LOW:						severity_msg = "Severity: low"; break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:	severity_msg = "Severity: notification"; break;
      default:															severity_msg = "Severity: Unknown"; break;
    }

    switch (severity)
    {
      case GL_DEBUG_SEVERITY_HIGH:						SPG_CRITICAL("{} {} {}", severity_msg, type_msg, source_msg); break;
      case GL_DEBUG_SEVERITY_MEDIUM:					SPG_ERROR("{} {} {}", severity_msg, type_msg, source_msg); break;
      case GL_DEBUG_SEVERITY_LOW:							SPG_WARN("{} {} {}", severity_msg, type_msg, source_msg); break; 
      case GL_DEBUG_SEVERITY_NOTIFICATION:		SPG_INFO("{} {} {}", severity_msg, type_msg, source_msg); break; 
      default:																SPG_TRACE("{} {} {}", severity_msg, type_msg, source_msg); break; 
    }
    SPG_WARN("-----------------------------------------------------");
  }


  namespace Graphics_CTX_V2 //AI
  {
    
   // #include "GraphicsContext.hpp"
  // #include <stdexcept>
  // #include <iostream>

    GraphicsContext::GraphicsContext(int width, int height, const std::string& title)
        : m_Width(width), m_Height(height), m_Title(title) {
        Init();
    }

    GraphicsContext::~GraphicsContext() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
        }
        glfwTerminate();
    }

    void GraphicsContext::Init() {
        // 1. Initialize GLFW
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // 2. Configure OpenGL versions
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // Or 3.3 depending on your hardware
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // 3. Create the window
        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
        if (!m_Window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        // 4. Bind the context to the current thread
        MakeCurrent();

        // 5. Initialize GLAD loaders
        // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        //     throw std::runtime_error("Failed to initialize GLAD");
        // }

        // 6. Set initial viewport
        glViewport(0, 0, m_Width, m_Height);
      }

      void GraphicsContext::MakeCurrent() {
          glfwMakeContextCurrent(m_Window);
      }

      void GraphicsContext::SwapBuffers() {
          glfwSwapBuffers(m_Window);
      }

      void GraphicsContext::PollEvents() {
          glfwPollEvents();
      }

      bool GraphicsContext::ShouldClose() const {
          return glfwWindowShouldClose(m_Window);
      }
  }

}