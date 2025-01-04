
#include <GLFW/glfw3.h>


#include "Core/Base.h"
#include "Core/Window.h"
#include "ImGuiLayer.h"

namespace Spg
{
  void ImGuiLayer::Initialise(const Window& window)
  {
    SPG_ASSERT(window.GetWindowHandle() != nullptr);
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
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
  }

  void ImGuiLayer::Shutdown()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiLayer::PreRender()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();       
  }

  void ImGuiLayer::PostRender()
  {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
  }

  bool ImGuiLayer::WantCaptureMouse()
  {
    auto& io = ImGui::GetIO();
    return (bool)(io.WantCaptureMouse);
  }

  bool ImGuiLayer::WantCaptureKeyboard()
  {
    auto& io = ImGui::GetIO();
    return (bool)(io.WantCaptureKeyboard);
  }
}