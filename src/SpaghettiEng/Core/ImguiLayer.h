# pragma once

#include "SpaghettiEng/Core/Layer.h"

/*
  {} []
*/

namespace Spg
{
  class ServiceLocator;
  class Window;

  namespace WinEvt { struct Event; }

  class ImguiLayer : public Layer
  {
  public:
    ImguiLayer(ServiceLocator& service_locator, const std::string& name = "Default Layer");
    virtual ~ImguiLayer();

    virtual void Update(double delta_time) override {}
    virtual void Render(double delta_time) override;
    virtual void OnEvent(WinEvt::Event& event) override;
   
  protected:
    
    virtual void Draw(double delta_time) = 0;

    void Init();
    void Shutdown();
    
    void PreRender();
    void PostRender();

    bool WantCaptureMouse();
    bool WantCaptureKeyboard();

    void SetDarkThemeColors();
    
  };
}
