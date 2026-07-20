#pragma once

#include <vector>

#include "SpaghettiEng/Core/ServiceLocator.h"

namespace Spg
{
  namespace WinEvt { struct Event; }
  
  class Layer
  {

  public:
    Layer(ServiceLocator& app_context, const std::string& name = "Default Layer");
    virtual ~Layer() = default;

    virtual void Update(double delta_time) {}
    virtual void Render(double delta_time) {} 
    virtual void ImGuiRender() {} 
    virtual void OnEvent(WinEvt::Event& event) {};

    const std::string& GetName() const { return m_name; }
    void SetEnabled(bool val) {m_enabled = val;}
    bool IsEnabled() const { return m_enabled; }

  protected:
    ServiceLocator& m_app_conext;
    std::string m_name;
    bool m_enabled = true;
  };

  class LayerStack
  {
  public:
    LayerStack() = default;
    ~LayerStack();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);
    void PopLayer(Layer* layer);
    void PopOverlay(Layer* overlay);

    auto begin() { return m_Layers.begin(); }
    auto end() { return m_Layers.end(); }
    auto rbegin() { return m_Layers.rbegin(); }
    auto rend() { return m_Layers.rend(); }

    auto begin() const { return m_Layers.cbegin(); }
    auto end()	const { return m_Layers.cend(); }
    auto rbegin() const { return m_Layers.crbegin(); }
    auto rend() const { return m_Layers.crend(); }

  private:
    std::vector<Layer*> m_Layers;
    uint32_t m_LayerInsertIndex = 0;
  };

}