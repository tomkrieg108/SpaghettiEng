#pragma once

#include <memory>

#include "SpaghettiEng/Scene/Entity.h"

namespace Spg
{
  class Registry;
  class SceneCameraController;
   
  class Scene
  {
    public:
      Scene();
      ~Scene();

      Entity CreateEmpty(const std::string& name);
      Entity CreatePlane(const std::string& name);
      Entity CreateCube(const std::string& name);
      Entity CreateSphere(const std::string& name);

   
    private:
      //Use pointers here to avoid #including Registry.h (<entt/entt.hpp>) in the header - big compile time savings!
      //Scene members will only be called a few times per frame - no performance issue
      


      std::unique_ptr<Registry> m_registry;
      std::unique_ptr<SceneCameraController> m_scene_camera_controller;

      //todo:  consider the pimpl idiom:
      //std::unique_ptr<Impl> m_impl; 
  };

  
}