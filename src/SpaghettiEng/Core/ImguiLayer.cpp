#include "SpaghettiEng/Core/ImguiLayer.h"

#include <string>
#include <filesystem>

#include <GLFW/glfw3.h> //Needed in PostRender()
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "CoreLib/Core.h"
#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/Layer.h"
#include "SpaghettiEng/Core/ServiceLocator.h"
#include "SpaghettiEng/Resource/ResourceManager.h"

/*
  {} []
*/

namespace Spg
{

  namespace fs = std::filesystem;

  ImguiLayer::ImguiLayer(ServiceLocator& service_locator, const std::string& name) :
    Layer(service_locator, name)
  {
    Init();
  }

  ImguiLayer::~ImguiLayer()
  {
    Shutdown();
  }

  void ImguiLayer::Init()
  {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO(); 
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  #ifdef WIN32
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport
  #endif

    // Set fonts
    float fontSize = 22.0f; 	//NOTE - different fonts can be downloaded from google fonts
    auto assets_path = m_service_locator.Get<ResourceManager>().GetAssetsPath();
    auto font_bold = assets_path / fs::path{"Fonts/Opensans/OpenSans-Bold.ttf"};
    auto font_regular = assets_path / fs::path{"Fonts/Opensans/OpenSans-Regular.ttf"};
    
    if(!fs::is_regular_file(font_bold))
      SPG_ERROR("Cannot find font: {}", font_bold.string());

    if(!fs::is_regular_file(font_regular))
      SPG_ERROR("Cannot find font: {}", font_regular.string());

    io.Fonts->AddFontFromFileTTF(font_bold.string().c_str(), fontSize);
    io.Fonts->AddFontFromFileTTF(font_regular.string().c_str(), fontSize);
    ImGui::StyleColorsDark();

    //Set Theme Colours
    SetDarkThemeColors();

    // Setup Platform/Renderer backends
    const auto& window = m_service_locator.Get<Window>();
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
  }

  void ImguiLayer::Shutdown()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImguiLayer::Render(double delta_time)
  {
    PreRender();
    Draw(delta_time);
    PostRender();
  }

  void ImguiLayer::OnEvent(WinEvt::Event& event)
  {
    if(WantCaptureKeyboard() || WantCaptureMouse())
      event.handled = true;
  }

  void ImguiLayer::PreRender()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();       
  } 

  void ImguiLayer::PostRender()
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
  
  bool ImguiLayer::WantCaptureMouse()
  {
    auto& io = ImGui::GetIO();
    return (bool)(io.WantCaptureMouse);
  } 

  bool ImguiLayer::WantCaptureKeyboard()
  {
    auto& io = ImGui::GetIO();
    return (bool)(io.WantCaptureKeyboard);
  } 

  void ImguiLayer::SetDarkThemeColors()
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

