

//#include "AppContext.h"
#include "Layer.h"

namespace Spg
{
  Layer::Layer(AppContext& app_context, const std::string& name)
	: m_app_conext{app_context}, m_name(name)
  {
  }

  LayerStack::~LayerStack()
  {
    for (Layer* layer : m_Layers)
    {
      layer->OnDetach();
      delete layer;
    }
  }

  void LayerStack::PushLayer(Layer* layer)
  {
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
  }

  void LayerStack::PushOverlay(Layer* overlay)
  {
    m_Layers.emplace_back(overlay);
  }

  void LayerStack::PopLayer(Layer* layer)
  {
    auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
    if (it != m_Layers.begin() + m_LayerInsertIndex)
    {
      layer->OnDetach();
      m_Layers.erase(it);
      m_LayerInsertIndex--;
    }
  }

  void LayerStack::PopOverlay(Layer* overlay)
  {
    auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
    if (it != m_Layers.end())
    {
      overlay->OnDetach();
      m_Layers.erase(it);
    }
  }
}