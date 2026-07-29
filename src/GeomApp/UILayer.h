#pragma once
#include <string>

#include "SpaghettiEng/Core/ImguiLayer.h"
/*
  {} []
*/
namespace Spg
{
  class ServiceLocator;

  class UILayer : public ImguiLayer
  {
    public:
      UILayer(ServiceLocator& service_locator, const std::string& name);
      ~UILayer() = default;

    private:
      void Draw(double delta_time) override;   
  };

}