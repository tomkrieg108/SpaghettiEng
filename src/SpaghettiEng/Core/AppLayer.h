#pragma once

#include "Window.h"
#include "Layer.h"

namespace Spg
{
  class AppLayer : public Layer
  {
  public:
    AppLayer(AppContext& app_context, const std::string& name);
    ~AppLayer() = default;

    void OnAttach() override {};
    void OnDetach() override {};
    void OnUpdate(double time_step) override {};
    void OnEvent(Event& event) override {};
    void OnImGuiRender() override;  
  private:
    Window& m_window;  
  };

}