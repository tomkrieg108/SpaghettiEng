#include "ImGuiUtils/ImGuiUtils.h"
#include "AppContext.h"
#include "AppLayer.h"

namespace Spg
{
  AppLayer::AppLayer(AppContext& app_context, const std::string& name) : 
    Layer(app_context, name), m_window(app_context.Get<Window>("Window"))
  {
  }

  void AppLayer::OnImGuiRender()
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