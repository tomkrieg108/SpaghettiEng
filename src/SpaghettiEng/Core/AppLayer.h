#pragma once

#include "SpaghettiEng/Core/Layer.h"
#include "SpaghettiEng/Core/WindowEvents.h"

namespace Spg
{
  class Window;
  
  class AppLayer : public Layer
  {
  public:
    AppLayer(ServiceLocator& app_context, const std::string& name);
    ~AppLayer() = default;

    void ImGuiRender() override;  
  private:
    Window& m_window;  
  };

}