
#include "GeomApp/UILayer.h"

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
  UILayer::UILayer(ServiceLocator& service_locator, const std::string& name) :
    ImguiLayer(service_locator, name)
  {} 

  void UILayer::Draw(double delta_time)
  {
    Window& window = m_service_locator.Get<Window>();

    ImGui::Begin(m_name.c_str());
    
    


    ImGui::ShowDemoWindow();
  }

}