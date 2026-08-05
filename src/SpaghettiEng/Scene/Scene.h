#pragma once

#include <memory>
#include <utility>
#include <string>

#include "SpaghettiEng/Scene/Entity.h"

// {} []
namespace Spg
{
  // Forward decl to maintain compile guard for <entt/entity/registry.hpp>
  // avoids including Registry.h (and <entt/entity/registry.hpp> )
  class Registry; 
  
  class Scene
  {
    public:
      Scene();
      ~Scene(); // User defined destructor => move ctr's needed

      Scene(Scene&& scene) noexcept;
      Scene& operator=(Scene&& scene) noexcept;

      Scene(const Scene&) = delete;            
      Scene& operator=(const Scene&) = delete;

    // Maybe keep this stuff out of Scene
    #if 0
      void OnSimulationStart() {}
		  void OnSimulationStop() {}
      bool IsRunning() const {}
		  bool IsPaused() const {}
		  void SetPaused(bool paused) {}
		  void Step(int frames = 1) {} 
    #endif

      

      // Entity CreateEmpty(const std::string& name);
      // Entity CreatePlane(const std::string& name);
      // Entity CreateCube(const std::string& name);
      // Entity CreateSphere(const std::string& name);

      Registry& GetRegistry() { return *m_registry; }
      const Registry& GetRegistry() const { return *m_registry; }
      Registry* GetRegistryPtr() { return m_registry.get(); }

    private:
     
      std::unique_ptr<Registry> m_registry;

  };

  
}