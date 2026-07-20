#pragma once

#include <memory>

#include "SpaghettiEng/Scene/Entity.h"

// {}
namespace Spg
{
  class Registry;
  
  class Scene
  {
    public:
      Scene();
      ~Scene();

      Scene(Scene&& scene) noexcept;
      Scene& operator=(Scene&& scene) noexcept;

      Scene(const Scene&) = delete;            
      Scene& operator=(const Scene&) = delete;

      void OnSimulationStart() {}
		  void OnSimulationStop() {}
      bool IsRunning() const {}
		  bool IsPaused() const {}

		  void SetPaused(bool paused) {}
		  void Step(int frames = 1) {} 

      // Entity CreateEmpty(const std::string& name);
      // Entity CreatePlane(const std::string& name);
      // Entity CreateCube(const std::string& name);
      // Entity CreateSphere(const std::string& name);

      Registry& GetRegistry() { return *m_registry; }
      Registry* GetRegistryPtr() { return m_registry.get(); }

    private:
     
      std::unique_ptr<Registry> m_registry;

  };

  
}