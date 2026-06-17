#pragma once

#include <cstdint>
#include <memory>

namespace Spg
{
  class Registry;

  class Scene
  {
    public:
      Scene();
      ~Scene();
    private:
      std::unique_ptr<Registry> m_registry;
  };

  
}