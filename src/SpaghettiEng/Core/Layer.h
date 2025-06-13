#pragma once
#include "AppContext.h"

namespace Spg
{
  struct Event;
 
  class Layer
  {
  public:
    Layer(AppContext& app_context, const std::string& name = "Default Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(double delta_time) {}
    virtual void OnEvent(Event& event) {};
    virtual void OnImGuiRender() {}
    const std::string& GetName() const { return m_name; }
  protected:
    AppContext& m_app_conext;
    std::string m_name;
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