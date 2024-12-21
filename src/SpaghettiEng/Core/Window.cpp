

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Log.h"
#include "Window.h"

namespace Spg
{
  constexpr auto WINDOW_WIDTH = std::uint32_t{1280};
  constexpr auto WINDOW_HEIGHT = std::uint32_t{720};

  static void ErrorCallback(int error, const char* description)
  {
	  LOG_ERROR("GLFW Error: {}", description);
  }

  Window::Window()
  {
  }

  Window::~Window()
  {
  }

  void Window::Initialise()
  {
    glfwSetErrorCallback(ErrorCallback);

    if(!glfwInit())
    {
      std::cout << "GLFW initalisation failed\n";
      glfwTerminate();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		//glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
		glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);

    m_window_handle = glfwCreateWindow(static_cast<std::int32_t>(WINDOW_WIDTH),
                                    static_cast<std::int32_t>(WINDOW_HEIGHT),
                                    "Gui",
                                    nullptr,
                                    nullptr);


    if (!m_window_handle)
		{
			std::cout << "GLFW window creation failed\n";
			glfwTerminate();
		}

    glfwMakeContextCurrent(m_window_handle);
		int status = gladLoadGL(glfwGetProcAddress);
		if (!status)
		{
      std::cout << "Failed to initialise OpenGL context\n";
			glfwDestroyWindow(m_window_handle);
			glfwTerminate();
		}

    int32_t buff_width = 0;
    int32_t buff_height = 0;
    glfwGetFramebufferSize(m_window_handle, &buff_width, &buff_height);
    glViewport(0, 0, buff_width, buff_height);
    glfwSwapInterval(1); //vsync enabled
    glfwSetInputMode(m_window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO(); 
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport
		float fontSize = 20.0f; 	//NOTE - different fonts can be downloaded from google fonts

    // Setup Platform/Renderer backends
    const char *glsl_version = "#version 150";
    ImGui_ImplGlfw_InitForOpenGL(m_window_handle, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glfwSetWindowUserPointer(m_window_handle, this);  //Used for setup of event handlers
    SetWindowEventCallbacks();
   
    LOG_INFO("WINDOW CREATED");
    LOG_WARN("BuffWidth: {}, BuffHeight: {}", buff_width, buff_height);
  }

  
  void Window::SetWindowEventCallbacks()
  {
    glfwSetWindowCloseCallback(m_window_handle, [](GLFWwindow* handle) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Window closed");
    });

    glfwSetWindowSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      LOG_TRACE("Window Resize {} {}", width, height); 
    });

    glfwSetFramebufferSizeCallback(m_window_handle, [](GLFWwindow* handle, int width, int height){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      LOG_TRACE("Frame Buffer Resize {} {}", width, height);        
    });

    glfwSetWindowPosCallback(m_window_handle, [](GLFWwindow* handle, int xpos, int ypos) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));  
      LOG_TRACE("Window moved {} {}", xpos, ypos);
    });

    glfwSetWindowIconifyCallback(m_window_handle, [](GLFWwindow* handle, int iconified){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Window iconify changed {}", iconified);  
    });

    glfwSetWindowMaximizeCallback(m_window_handle, [](GLFWwindow* handle, int maximised){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Window maximised changed {}", maximised);  
    });

    glfwSetCursorEnterCallback(m_window_handle, [](GLFWwindow* handle, int entered) {
		  Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Cursor enter/exit window {}", entered);  
		});

    glfwSetWindowFocusCallback(m_window_handle, [](GLFWwindow* handle, int focused){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Cursor focus changed {}", focused);  
    });

    glfwSetKeyCallback(m_window_handle, [](GLFWwindow* handle, int key, int code, int action, int mode){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Key pressed/released: {}", key, code, action, mode);  
      std::cout << "key\n";
    });

    glfwSetCharCallback(m_window_handle, [](GLFWwindow* handle, unsigned int keycode){
       Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
       LOG_TRACE("Char typed: {}", keycode);  
    });
    
    glfwSetMouseButtonCallback(m_window_handle, [](GLFWwindow* handle, int button, int action, int mods)
    {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Mouse button pressed/released: {}", button, action, mods);  
    });

    glfwSetScrollCallback(m_window_handle, [](GLFWwindow* handle, double xoffset, double yoffset){
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Mouse scrolled: {}", xoffset, yoffset);  
    });

    glfwSetCursorPosCallback(m_window_handle, [](GLFWwindow* handle, double xpos, double ypos) {
		  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(handle));
      LOG_TRACE("Cursor moved: {}", xpos, ypos); 
    });
  }

  void Window::RenderLoop()
  {
    while (!glfwWindowShouldClose(m_window_handle))
    {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();

      ImGui::NewFrame();
      ImGui::ShowDemoWindow();
      ImGui::Render();

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		  {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
		  }

      glfwSwapBuffers(m_window_handle);
      glfwPollEvents();
    }
  }

  void Window::Shutdown()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window_handle);
    glfwTerminate();
  }

}