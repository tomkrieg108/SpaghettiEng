#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "ImGuiLayer/ImGuiLayer.h"
#include "Events/EventManager.h"
#include "Log.h"
#include "Window.h"

namespace Spg
{
  static void ErrorCallback(int error, const char* description)
  {
	  LOG_ERROR("GLFW Error: {}", description);
  }

  Window::Window()
  {
    Initialise();
  }

  Window::~Window()
  {
  }

  void Window::Initialise()
  {
    glfwSetErrorCallback(ErrorCallback);

    if(!glfwInit())
    {
      LOG_ERROR("GLFW initalisation failed");
      glfwTerminate();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
		glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);

    m_window_handle = glfwCreateWindow(m_params.width,m_params.height,m_params.title.c_str(),nullptr,nullptr);
    if (!m_window_handle)
		{
      LOG_ERROR("GLFW window creation failed");
			glfwTerminate();
		}

    glfwMakeContextCurrent(m_window_handle);
		int status = gladLoadGL(glfwGetProcAddress);
		if (!status)
		{
      LOG_ERROR("OpenGL Initialisation failed");
			glfwDestroyWindow(m_window_handle);
			glfwTerminate();
		}

    glfwGetWindowSize(m_window_handle, &m_params.width, &m_params.height);
    glfwGetFramebufferSize(m_window_handle, &m_params.buffer_width, &m_params.buffer_height);
    glViewport(0, 0, m_params.buffer_width, m_params.buffer_height);

    SetVSyncEnabled(m_params.vsync_enabled);
    SetCursorEnabled(m_params.cursor_enabled);
    glfwSetWindowUserPointer(m_window_handle, this);  
    SetWindowEventCallbacks();

    LOG_INFO("WINDOW CREATED");
    LOG_WARN("BuffWidth: {}, BuffHeight: {}", m_params.buffer_width, m_params.buffer_height);
    LOG_WARN("Width: {}, Height: {}", m_params.width, m_params.height);
  }

  void Window::UpdateSize() {glfwGetFramebufferSize(m_window_handle, &m_params.buffer_width, &m_params.buffer_height);}
  void Window::PollEvents() const {glfwPollEvents();}
  void Window::SwapBuffers() const {glfwSwapBuffers(m_window_handle);}
  void Window::MakeContextCurrent() const {glfwMakeContextCurrent(m_window_handle);}
  const Window::Params& Window::GetParams() const {return m_params;}
  bool Window::IsVSyncEnabled() const { return m_params.vsync_enabled; }
  bool Window::IsCursorEnabled() const { return m_params.cursor_enabled; }
  bool Window::IsMinimised() const {return (bool)glfwGetWindowAttrib(m_window_handle, GLFW_ICONIFIED);}
  bool Window::IsMaximised() const {return (bool)glfwGetWindowAttrib(m_window_handle, GLFW_MAXIMIZED);}
  GLFWwindow* Window::GetWindowHandle() const {return m_window_handle;}

  void Window::ClearBuffers() const
  {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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

  void Window::Shutdown()
  {
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();

    glfwDestroyWindow(m_window_handle);
    glfwTerminate();

    #ifdef SPG_DEBUG
      LOG_INFO("Max event queue size: {}", EventManager::GetMaxQueueSize());
      LOG_INFO("Current event queue size: {}", EventManager::GetCurrentQueueSize());
    #endif    
  }
  
  void Window::SetWindowEventCallbacks()
  {
    glfwSetWindowCloseCallback(m_window_handle, [](GLFWwindow* handle) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      EventWindowClose e;
      EventManager::Dispatch(e);
      //EventManager::Enqueue(e);
      //LOG_TRACE("Window closed");
    });

    glfwSetWindowSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      //EventWindowResize e{width,height};
      //EventManager::Enqueue(e);
      //LOG_TRACE("Window Resize {} {}", width, height); 
    });

    glfwSetFramebufferSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      window->UpdateSize();
      EventWindowResize e{width,height};
      EventManager::Dispatch(e);
      //EventManager::Enqueue(e);
      //LOG_TRACE("Frame Buffer Resize {} {}", width, height);        
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
      // if(ImGui::GetIO().WantCaptureKeyboard)
      //   return;
      if(ImGuiLayer::WantCaptureKeyboard())  
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
      LOG_TRACE("Key pressed/released: {}", key);  
    });

    glfwSetCharCallback(m_window_handle, [](GLFWwindow* handle, unsigned int keycode){
       Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
       LOG_TRACE("Char typed: {}", keycode);  
    });
    
    glfwSetMouseButtonCallback(m_window_handle, [](GLFWwindow* handle, int button, int action, int mods)
    {
      // if (ImGui::GetIO().WantCaptureMouse)
	    //   return;
      if(ImGuiLayer::WantCaptureMouse())  
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
      // if (ImGui::GetIO().WantCaptureMouse)
	    //   return;
      if(ImGuiLayer::WantCaptureMouse())  
        return;
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      EventMouseScrolled e{(float)xoffset,(float)yoffset};
      EventManager::Enqueue(e);
    });

    glfwSetCursorPosCallback(m_window_handle, [](GLFWwindow* handle, double xpos, double ypos) {
      // if (ImGui::GetIO().WantCaptureMouse)
	    //    return;
      if(ImGuiLayer::WantCaptureMouse())  
        return;   
		  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      EventMouseMoved e{(float)xpos,(float)ypos,0,0};
      EventManager::Enqueue(e);
    });
  }

}