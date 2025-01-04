#pragma once
#include "SpaghettiEng/SpaghettiEng.h"

namespace Spg
{
  class DefaultLayer : public Layer
  {
  public:
    DefaultLayer(const std::string& name);
    ~DefaultLayer() = default;

    void OnAttach() override {};
    void OnDetach() override {};
    void OnUpdate(double time_step) override {};
    void OnEvent(Event& event) override {};
    void OnImGuiRender() override;  
  };

  class BubbleSoup : public Application
  {
  public:
    BubbleSoup(const std::string& title);
  private:
    Layer* m_default_layer = nullptr;
  };

  void AppPrintHello();
}
