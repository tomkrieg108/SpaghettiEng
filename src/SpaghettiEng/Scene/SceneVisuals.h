#pragma once

#include <memory>

#include "SpaghettiEng/Scene/Entity.h"

// {}
namespace Spg
{
  class Registry;

  class SceneVisuals
  {
    public:
      SceneVisuals();
      ~SceneVisuals();

      static SceneVisuals CreateDefault();

    private:
      Registry* m_registry = nullptr;
  };
}