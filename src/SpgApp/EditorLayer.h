#pragma once

#include <string>

#include "SpaghettiEng/Core/ImguiLayer.h"
/*
  {} []
*/
namespace Spg
{
  class ServiceLocator;

  class EditorLayer : public ImguiLayer
  {
    public:
      EditorLayer(ServiceLocator& service_locator, const std::string& name);
      ~EditorLayer() = default;

    private:
      void Draw(double delta_time) override;   
  };

}
