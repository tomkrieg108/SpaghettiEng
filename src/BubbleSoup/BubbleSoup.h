#pragma once
#include "SpaghettiEng/SpaghettiEng.h"
#include "SpaghettiEng/Camera/Camera2D.h"

using namespace std::string_literals;

namespace Spg
{
  class DefaultLayer : public Layer
  {
  public:
    DefaultLayer(const AppContext& app_context, const std::string& name);
    ~DefaultLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(double time_step) override {};
    void OnEvent(Event& event) override {};
    void OnImGuiRender() override;  

  private:
    void OnMouseMoved(EventMouseMoved& e);
    void OnMouseButtonPressed(EventMouseButtonPressed& e);
    void OnMouseButtonReleased(EventMouseButtonReleased& e);

    Utils::SpdLogger m_logger;
  };

  class BubbleSoup : public Application
  {
  public:
    BubbleSoup(const std::string& title);
    ~BubbleSoup();
  private:
    Layer* m_default_layer = nullptr;
    Camera2D m_camera;
  };

  void AppPrintHello();
}
