#pragma once

#include "SpaghettiEng/Core/Layer.h"
#include "SpaghettiEng/Core/WindowEvents.h"

// {} []

namespace Spg
{
  class Window;
  class ServiceLocator;
  class GLRenderer;
  class SceneManager;

  class SimLayer : public Layer
  {
   public:
    SimLayer(ServiceLocator& service_locator, const std::string& name);
    ~SimLayer() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update(double delta_time) override;
    virtual void Render(double delta_time) override;
    virtual void OnEvent(WinEvt::Event& event) override;

   private:
    void OnWindowResize(WinEvt::WindowResize& e);
    void OnMouseMoved(WinEvt::MouseMoved& e);
    void OnMouseScrolled(WinEvt::MouseScrolled& e);
    void OnMouseButtonPressed(WinEvt::MouseBtnPressed& e);  

  private:
    const Window& m_window;
    SceneManager& m_scene_mgr;
    GLRenderer& m_renderer;
  };

}
