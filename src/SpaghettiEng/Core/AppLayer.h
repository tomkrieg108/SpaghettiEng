#pragma once

#include "Layer.h"

namespace Spg
{
  class AppLayer : public Layer
  {
  public:
    AppLayer(const AppContext& app_context, const std::string& name);
    ~AppLayer() = default;

    void OnAttach() override {};
    void OnDetach() override {};
    void OnUpdate(double time_step) override {};
    void OnEvent(Event& event) override {};
    void OnImGuiRender() override;  
  };

}