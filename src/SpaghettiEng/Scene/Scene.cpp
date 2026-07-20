
#include "SpaghettiEng/Scene/Scene.h"

#include <memory>

#include "SpaghettiEng/Scene/Components.h"
#include "SpaghettiEng/Scene/Entity.h"
#include "SpaghettiEng/Scene/Registry.h"
#include "SpaghettiEng/Scene/SceneCameraController.h"

namespace Spg
{
  
  Scene::Scene() : m_registry(std::make_unique<Registry>()) {}
  Scene::~Scene() = default;

  Scene::Scene(Scene&& scene) noexcept = default;
  Scene& Scene::operator=(Scene&& scene) noexcept = default;

#if 0
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
#endif


}