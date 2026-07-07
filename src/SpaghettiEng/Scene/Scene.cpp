
#include "SpaghettiEng/Scene/Scene.h"

#include <memory>

#include "SpaghettiEng/Scene/Components.h"
#include "SpaghettiEng/Scene/Entity.h"
#include "SpaghettiEng/Scene/Registry.h"
#include "SpaghettiEng/Scene/SceneCameraController.h"

namespace Spg
{
  //pimpl idiom
  //contains all instance members (packed tightly).  Define here
  /*
  struct Impl
  {
    Registry registry;
    SceneCameraController scene_camera_controller
  };

  Scene::Scene() : m_impl(std::make_unique<Impl>()) {}
  */ 

  Scene::Scene() : 
  m_registry(std::make_unique<Registry>()),
  m_scene_camera_controller(std::make_unique<SceneCameraController>())
  {
  }

  Scene::~Scene() = default;

  Entity Scene::CreateEmpty(const std::string& name)
  {
    Entity entity = m_registry->CreateEntity();
    return entity;
  }

  Entity Scene::CreatePlane(const std::string& name)
  {
    Entity entity = CreateEmpty(name);
    m_registry->AddComponent<TransformComponent>(entity);
    m_registry->AddComponent<MeshComponent>(entity);
    return entity;
  }

  Entity Scene::CreateCube(const std::string& name)
  {
    Entity entity = CreateEmpty(name);
    m_registry->AddComponent<TransformComponent>(entity);
    m_registry->AddComponent<MeshComponent>(entity);
    return entity;
  }

  Entity Scene::CreateSphere(const std::string& name)
  {
    Entity entity = CreateEmpty(name);
    m_registry->AddComponent<TransformComponent>(entity);
    m_registry->AddComponent<MeshComponent>(entity);
    return entity;
  }



}