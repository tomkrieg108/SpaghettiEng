
#include "SpgApp/EditorLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "SpaghettiEng/Core/ImguiLayer.h"
#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/ServiceLocator.h"

/*
  {} []
*/

namespace Spg
{
  EditorLayer::EditorLayer(ServiceLocator& service_locator, const std::string& name) :
    ImguiLayer(service_locator, name)
  {} 

  void EditorLayer::Draw(double delta_time)
  {
    Window& window = m_service_locator.Get<Window>();

    ImGui::Begin(m_name.c_str());
    if (ImGui::CollapsingHeader("Window: Size,Framerate"))
    {
      Window::Params& params = window.GetParams();

      ImGui::Text("Width,Height %d %d : ", params.width, params.height);
      ImGui::Text("Buff Width, Buff Height %d %d : ", params.buffer_width, params.buffer_height);
      ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().
      Framerate);
      ImGui::Checkbox("VSync Enable: ", &params.vsync_enabled);
      window.SetVSyncEnabled(params.vsync_enabled);
    }
    ImGui::End();
    ImGui::ShowDemoWindow();
  }

}