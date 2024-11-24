

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Log.h"
#include "Window.h"

namespace spg
{
  constexpr auto WINDOW_WIDTH = std::uint32_t{1280};
  constexpr auto WINDOW_HEIGHT = std::uint32_t{720};

  Window::Window()
  {
    Log::Init();
  }

  Window::~Window()
  {
    glfwDestroyWindow(m_handle);
    glfwTerminate();
  }

  void Window::Init()
  {
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

    m_handle = glfwCreateWindow(static_cast<std::int32_t>(WINDOW_WIDTH),
                                    static_cast<std::int32_t>(WINDOW_HEIGHT),
                                    "Gui",
                                    nullptr,
                                    nullptr);


    if (!m_handle)
		{
			std::cout << "GLFW window creation failed\n";
			glfwTerminate();
		}

    glfwMakeContextCurrent(m_handle);
		int status = gladLoadGL(glfwGetProcAddress);
		if (!status)
		{
      std::cout << "Failed to initialise OpenGL context\n";
			glfwDestroyWindow(m_handle);
			glfwTerminate();
		}

    int32_t buff_width = 0;
    int32_t buff_height = 0;
    glfwGetFramebufferSize(m_handle, &buff_width, &buff_height);
    glViewport(0, 0, buff_width, buff_height);
    glfwSwapInterval(1); //vsync enabled
    glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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
    ImGui_ImplGlfw_InitForOpenGL(m_handle, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    LOG_INFO("WINDOW CREATED:");
    LOG_WARN("BuffWidth: {}, BuffHeight: {}", buff_width, buff_height);
  }

  void Window::RenderLoop()
  {
    while (!glfwWindowShouldClose(m_handle))
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

      glfwSwapBuffers(m_handle);
      glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_handle);
    glfwTerminate();
  }

}