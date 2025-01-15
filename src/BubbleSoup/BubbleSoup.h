#pragma once
#include <SpaghettiEng/SpaghettiEng.h>
#include <SpaghettiEng/Camera/Camera2D.h>
#include <SpaghettiEng/OpenGL32/GL32Renderer.h>

using namespace std::string_literals;

namespace Spg
{
  class BubbleLayer : public Layer
  {
  public:
    BubbleLayer(AppContext& app_context, const std::string& name);
    ~BubbleLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(double time_step) override {};
    void OnEvent(Event& event) override {};
    void OnImGuiRender() override;  

  private:
    void OnMouseMoved(EventMouseMoved& e);
    void OnMouseButtonPressed(EventMouseButtonPressed& e);
    void OnMouseButtonReleased(EventMouseButtonReleased& e);

  private:
    Window& m_window;
    GLRenderer& m_renderer;
    CameraController2D& m_camera_controller;
    
    Utils::SpdLogger m_logger;
  };

  class BubbleSoup : public Application
  {
  public:
    BubbleSoup(const std::string& title);
    ~BubbleSoup();
  private:
  
  };

  void AppPrintHello();
}
