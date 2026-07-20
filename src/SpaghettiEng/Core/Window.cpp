//#define GLFW_INCLUDE_NONE
#include "SpaghettiEng/Core/Window.h"

#include <glad/gl.h>  //Need to include before glfw3.h
#include <GLFW/glfw3.h>

#include "CoreLib/Logger.h"

#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/InputState.h"
#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/ImGuiUtils/ImGuiUtils.h"
#include "SpaghettiEng/Events/EventManager.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLContext.h"


namespace Spg
{
  static void ErrorCallback(int error, const char* description)
  {
	  SPG_ERROR("GLFW Error: {}", description);
  }

  uint32_t Window::s_window_count = 0;

  Window::Window(const std::string& title)
  {
    SPG_ASSERT(Window::s_window_count == 0);

    glfwSetErrorCallback(ErrorCallback);

    if(!glfwInit())
    {
      SPG_CRITICAL("GLFW initalisation failed");
      glfwTerminate();
      return;
    }
    //Todo. Linux does not work with V4.5 (V4.2 max?)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
		glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);

    m_window_handle = glfwCreateWindow(m_params.width,m_params.height,title.c_str(),nullptr,nullptr);
    if (!m_window_handle)
		{
      SPG_CRITICAL("GLFW window creation failed. Terminating!");
			glfwTerminate();
      return;
		}

    //calls glfwMakeContextCurrent(m_glfw_window_handle), initialized glad
    //needs to be called before glfwSwapInterval(1); in SetVSyncEnabled()
    m_graphics_context = new OpenGLContext(m_window_handle);
   
    m_params.title = title;
    glfwGetWindowSize(m_window_handle, &m_params.width, &m_params.height);
    glfwGetFramebufferSize(m_window_handle, &m_params.buffer_width, &m_params.buffer_height);

    SetVSyncEnabled(m_params.vsync_enabled);
    SetCursorEnabled(m_params.cursor_enabled);
    glfwSetWindowUserPointer(m_window_handle, this);  
    SetWindowEventCallbacks();

    SPG_INFO("Window Created.  Buffer width, height: {}, {}", m_params.buffer_width, m_params.buffer_height);

    s_window_count++;

    
    m_graphics_context->SetViewport(0, 0, m_params.buffer_width, m_params.buffer_height);

    m_input_state = new InputState(m_window_handle);
  }

  Window::~Window()
  {
    glfwDestroyWindow(m_window_handle);
    s_window_count--;

    if(m_graphics_context != nullptr)
      delete m_graphics_context;

    if(m_input_state != nullptr)
      delete m_input_state;  

    if(s_window_count == 0)
      glfwTerminate();
  }

  void Window::OnUpdate()
  {
    //glfwPollEvents(): Empties window even queue and triggers associated callback fns
    // Must be called in main thread during each frame
    glfwPollEvents(); 
    m_graphics_context->SwapBuffers();
  }

  bool Window::ShouldClose()
  {
    return static_cast<bool>(glfwWindowShouldClose(m_window_handle));
  }

  bool Window::IsMinimised() const 
  {
    return (bool)glfwGetWindowAttrib(m_window_handle, GLFW_ICONIFIED);
  }

  void Window::Clear() const
  {
    m_graphics_context->ClearBuffer();
  }

  float Window::GetAspectRatio() const
  {
    if(!IsMinimised() && m_params.buffer_height > 0)
      return (float)(m_params.buffer_width) / (float)(m_params.buffer_height);
    return 1.0f;  
  }

  void Window::SetVSyncEnabled(bool enable)
  {
    enable ? glfwSwapInterval(1) : glfwSwapInterval(0);
    m_params.vsync_enabled = enable;
  }

  void Window::SetCursorEnabled(bool enabled) 
  {
    #ifdef __linux__
        return;
    #endif
      if(enabled)
		    glfwSetInputMode(m_window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      else
		    glfwSetInputMode(m_window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      m_params.cursor_enabled = enabled;
  }

  void Window::SetWindowEventCallbacks()
  {
    glfwSetWindowCloseCallback(m_window_handle, [](GLFWwindow* handle) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      WinEvt::WindowClose e;
      auto& callback = window->GetEventCallback();
      callback(e);
    });

    glfwSetWindowSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      WinEvt::WindowResize e{width,height};
      auto& callback = window->GetEventCallback();
      callback(e);
    });

    glfwSetFramebufferSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      window->m_graphics_context->SetViewport(0,0,width,height);
      window->GetParams().buffer_width = width;
      window->GetParams().buffer_height = height;
      WinEvt::WindowResize e{width,height};
      auto& callback = window->GetEventCallback();
      callback(e);
    });

    glfwSetWindowPosCallback(m_window_handle, [](GLFWwindow* handle, int xpos, int ypos) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle)); 
      WinEvt::WindowMove e{xpos, ypos};
      auto& callback = window->GetEventCallback();
      callback(e);
    });

    glfwSetWindowIconifyCallback(m_window_handle, [](GLFWwindow* handle, int iconified){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      WinEvt::WindowIconifyChange e{ iconified };
      auto& callback = window->GetEventCallback();
      callback(e);
    });

    glfwSetWindowMaximizeCallback(m_window_handle, [](GLFWwindow* handle, int maximised){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      //SPG_TRACE("Window maximised"); 
    });

    glfwSetCursorEnterCallback(m_window_handle, [](GLFWwindow* handle, int entered) {
		  Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      //SPG_TRACE("Cursor enter"); 
		});

    glfwSetWindowFocusCallback(m_window_handle, [](GLFWwindow* handle, int focused){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      WinEvt::WindowFocusChange e{ focused };
      auto& callback = window->GetEventCallback();
      callback(e);
    });

    glfwSetKeyCallback(m_window_handle, [](GLFWwindow* handle, int key, int code, int action, int mode){
      if(ImGuiUtils::WantCaptureKeyboard())  
        return;

      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      auto& callback = window->GetEventCallback();
      if (action == GLFW_PRESS)
      {
        WinEvt::KeyPressed e{key};
        callback(e);
      }
      else if (action == GLFW_RELEASE)
      {
        WinEvt::KeyReleased e{key};
        callback(e); 
      }
      else if (action == GLFW_REPEAT)
      {
        WinEvt::KeyPressed e{key,true};
        callback(e);  
      } 
    });

    glfwSetCharCallback(m_window_handle, [](GLFWwindow* handle, unsigned int keycode){
       Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
       //SPG_TRACE("Char typed: {}", keycode);  
    });
    
    glfwSetMouseButtonCallback(m_window_handle, [](GLFWwindow* handle, int button, int action, int mods)
    {
      if(ImGuiUtils::WantCaptureMouse())  
        return;

      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      auto& callback = window->GetEventCallback();

      double xpos, ypos;
      glfwGetCursorPos(handle, &xpos, &ypos);

      if(action == GLFW_PRESS)
      {
        WinEvt::MouseBtnPressed e {(float)xpos, (float)ypos, button};
        callback(e);
      }
      if(action == GLFW_RELEASE)
      {
        WinEvt::MouseBtnReleased e {(float)xpos, (float)ypos, button};
        callback(e);
      }
    });

    glfwSetScrollCallback(m_window_handle, [](GLFWwindow* handle, double xoffset, double yoffset){
      if(ImGuiUtils::WantCaptureMouse())  
        return;
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      WinEvt::MouseScrolled e{(float)xoffset,(float)yoffset};
      auto& callback = window->GetEventCallback();
      callback(e);
    });

    glfwSetCursorPosCallback(m_window_handle, [](GLFWwindow* handle, double xpos, double ypos) {
      if(ImGuiUtils::WantCaptureMouse())  
        return; 

		  Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      auto* input = window->GetInputState();
      if(!input->GetMouseFirstMoved())
        input->SetMouseFirstMoved();

      float x_new = (float)(xpos);
      float y_new = (float)(ypos);

      //Todo - this will mostly be ignored - only queue if a mouse or key is also currently being pressed
      WinEvt::MouseMoved e{x_new,y_new,input->GetMouseDeltaX(x_new),input->GetMouseDeltaY(y_new)};
      auto& callback = window->GetEventCallback();
      callback(e);
    });
  }
}