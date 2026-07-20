
#include "SpaghettiEng/Core/AppLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/ServiceLocator.h"
#include "SpaghettiEng/ImGuiUtils/ImGuiUtils.h"

namespace Spg
{
  AppLayer::AppLayer(ServiceLocator& app_context, const std::string& name) : 
    Layer(app_context, name), m_window(app_context.Get<Window>())
  {
  }

  void AppLayer::ImGuiRender()
  {
    ImGui::Begin(m_name.c_str());
    if (ImGui::CollapsingHeader("Window: Size,Framerate"))
    {
      Window::Params& params = m_window.GetParams();

      ImGui::Text("Width,Height %d %d : ", params.width, params.height);
      ImGui::Text("Buff Width, Buff Height %d %d : ", params.buffer_width, params.buffer_height);
      ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().
      Framerate);
      ImGui::Checkbox("VSync Enable: ", &params.vsync_enabled);
      m_window.SetVSyncEnabled(params.vsync_enabled);
    }
    ImGui::End();
    ImGui::ShowDemoWindow();
  }

}