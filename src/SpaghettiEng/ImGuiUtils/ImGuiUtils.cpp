#include <filesystem>
#include <GLFW/glfw3.h>
#include "Core/Window.h"
#include "ImGuiUtils.h"

namespace Spg
{
  namespace fs = std::filesystem;

  void ImGuiUtils::Initialise(const Window& window)
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
		float fontSize = 22.0f; 	//NOTE - different fonts can be downloaded from google fonts

    auto font_bold = fs::absolute(fs::current_path() / fs::path{"Fonts/Opensans/OpenSans-Bold.ttf"});
    auto font_regular = fs::absolute(fs::current_path() / fs::path{"Fonts/Opensans/OpenSans-Regular.ttf"});
    
    if(!fs::is_regular_file(font_bold))
      SPG_ERROR("Cannor find font: {}", font_bold.string());

    if(!fs::is_regular_file(font_regular))
      SPG_ERROR("Cannor find font: {}", font_regular.string());

    io.Fonts->AddFontFromFileTTF(font_bold.string().c_str(), fontSize);
    io.Fonts->AddFontFromFileTTF(font_regular.string().c_str(), fontSize);
    ImGui::StyleColorsDark();
    ImGuiUtils::SetDarkThemeColors();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
  }

  void ImGuiUtils::Shutdown()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiUtils::PreRender()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();       
  }

  void ImGuiUtils::PostRender()
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

  bool ImGuiUtils::WantCaptureMouse()
  {
    auto& io = ImGui::GetIO();
    return (bool)(io.WantCaptureMouse);
  }

  bool ImGuiUtils::WantCaptureKeyboard()
  {
    auto& io = ImGui::GetIO();
    return (bool)(io.WantCaptureKeyboard);
  }

  void ImGuiUtils::SetDarkThemeColors()
  {
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  }
}