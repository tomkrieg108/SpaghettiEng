#include <GLFW/glfw3.h>
#include "ImGuiUtils/ImGuiUtils.h"
#include "Events/EventManager.h"
#include "OpenGL32/GL32Context.h"
#include "OpenGL32/GL32Renderer.h"
#include "Input.h"
#include "Window.h"

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
    }
    //Todo. Linux does not work with V4.5 (V4.2 max?)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
		}
     m_input = new Input(m_window_handle);

    m_graphics_context = new GLContext(m_window_handle);
    m_graphics_context->Initialise();

    m_params.title = title;
    glfwGetWindowSize(m_window_handle, &m_params.width, &m_params.height);
    glfwGetFramebufferSize(m_window_handle, &m_params.buffer_width, &m_params.buffer_height);

    GLRenderer::SetViewport(0, 0, m_params.buffer_width, m_params.buffer_height);
   
    SetVSyncEnabled(m_params.vsync_enabled);
    SetCursorEnabled(m_params.cursor_enabled);
    glfwSetWindowUserPointer(m_window_handle, this);  
    SetWindowEventCallbacks();

    SPG_INFO("Window Created.  Buffer width, height: {}, {}", m_params.buffer_width, m_params.buffer_height);

    s_window_count++;
  }

  Window::~Window()
  {
    glfwDestroyWindow(m_window_handle);
    s_window_count--;

    if(m_graphics_context != nullptr)
      delete m_graphics_context;

    if(m_input != nullptr)
      delete m_input;

    if(s_window_count == 0)
      glfwTerminate();
  }

  void Window::OnUpdate()
  {
    glfwPollEvents();
    m_graphics_context->SwapBuffers();
  }

  const Window::Params& Window::GetParams() const
  { 
    return m_params;
  }

  Window::Params& Window::GetParams()
  {
    return m_params;     
  }

  Input* Window::GetInput()
  {
    return m_input;
  }

  bool Window::IsVSyncEnabled() const 
  { 
    return m_params.vsync_enabled; 
  }

  bool Window::IsCursorEnabled() const 
  { 
    return m_params.cursor_enabled; 
  }

  bool Window::IsMinimised() const 
  {
    return (bool)glfwGetWindowAttrib(m_window_handle, GLFW_ICONIFIED);
  }

  GLFWwindow* Window::GetWindowHandle() const 
  {
    return m_window_handle;
  }

  void Window::Clear() const
  {
    GLRenderer::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    GLRenderer::ClearBuffers();
  }

  float Window::GetAspectRatio() const
  {
    SPG_ASSERT(m_params.buffer_height > 0);
    return (m_params.buffer_width) / (m_params.buffer_height);
  }

  void Window::SetVSyncEnabled(bool enable)
  {
    enable ? glfwSwapInterval(1) : glfwSwapInterval(0);
    m_params.vsync_enabled = enable;
  }

  void Window::SetCursorEnabled(bool enabled) 
  {
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
      EventWindowClose e;
      EventManager::Dispatch(e);
    });

    glfwSetWindowSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      //EventWindowResize e{width,height};
      //EventManager::Enqueue(e);
      //LOG_TRACE("Window Resize {} {}", width, height); 
    });

    glfwSetFramebufferSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      GLRenderer::SetViewport(0,0,width,height);  
      window->GetParams().buffer_width = width;
      window->GetParams().buffer_height = height;
      EventWindowResize e{width,height};
      EventManager::Dispatch(e);   
    });

    glfwSetWindowPosCallback(m_window_handle, [](GLFWwindow* handle, int xpos, int ypos) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      //EventWindowMove e{xpos, ypos};
      //EventManager::Enqueue(e);
      //LOG_TRACE("Window moved {} {}", xpos, ypos);
    });

    glfwSetWindowIconifyCallback(m_window_handle, [](GLFWwindow* handle, int iconified){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      //EventWindowIconifyChange e{ iconified };
      //EventManager::Enqueue(e);
      //LOG_TRACE("Window iconify changed {}", iconified);  
    });

    glfwSetWindowMaximizeCallback(m_window_handle, [](GLFWwindow* handle, int maximised){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      //LOG_TRACE("Window maximised changed {}", maximised);  
    });

    glfwSetCursorEnterCallback(m_window_handle, [](GLFWwindow* handle, int entered) {
		  Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      //EventWindowHoverChange e{entered};
      //EventManager::Enqueue(e);
      //LOG_TRACE("Cursor enter/exit window {}", entered);  
		});

    glfwSetWindowFocusCallback(m_window_handle, [](GLFWwindow* handle, int focused){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      //EventWindowFocusChange e{ focused };
      //EventManager::Enqueue(e);
      //LOG_TRACE("Cursor focus changed {}", focused);  
    });

    glfwSetKeyCallback(m_window_handle, [](GLFWwindow* handle, int key, int code, int action, int mode){
      if(ImGuiUtils::WantCaptureKeyboard())  
        return;

      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      if (action == GLFW_PRESS)
      {
        EventKeyPressed e{key};
        EventManager::Enqueue(e);    
      }
      else if (action == GLFW_RELEASE)
      {
        EventKeyReleased e{key};
        EventManager::Enqueue(e);    
      }
      else if (action == GLFW_REPEAT)
      {
        EventKeyPressed e{key,true};
        EventManager::Enqueue(e);     
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
      double xpos, ypos;
      glfwGetCursorPos(handle, &xpos, &ypos);

      if(action == GLFW_PRESS)
      {
        EventMouseButtonPressed e = EventMouseButtonPressed((float)xpos, (float)ypos, button);
        EventManager::Enqueue(e);
      }
      if(action == GLFW_RELEASE)
      {
        EventMouseButtonReleased e = EventMouseButtonReleased((float)xpos, (float)ypos, button);
        EventManager::Enqueue(e);
      }
    });

    glfwSetScrollCallback(m_window_handle, [](GLFWwindow* handle, double xoffset, double yoffset){
      if(ImGuiUtils::WantCaptureMouse())  
        return;
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      EventMouseScrolled e{(float)xoffset,(float)yoffset};
      EventManager::Enqueue(e);
    });

    glfwSetCursorPosCallback(m_window_handle, [](GLFWwindow* handle, double xpos, double ypos) {
      if(ImGuiUtils::WantCaptureMouse())  
        return; 

		  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      Input* input = win->GetInput();
      if(!input->GetMouseFirstMoved())
        input->SetMouseFirstMoved();

      float x_new = (float)(xpos);
      float y_new = (float)(ypos);
      EventMouseMoved e{x_new,y_new,input->GetMouseDeltaX(x_new),input->GetMouseDeltaY(y_new)};
      EventManager::Enqueue(e);
    });
  }
}