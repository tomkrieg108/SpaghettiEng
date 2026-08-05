#include "SpaghettiEng/Scene/SceneManager.h"

//#include <memory>

#include "CoreLib/Core.h"

#include "SpaghettiEng/Scene/Entity.h"
#include "SpaghettiEng/Scene/Registry.h"

//For BuildDefaultScene()

#include "SpaghettiEng/Resource/Resource.h"
#include "SpaghettiEng/Resource/ResourceCache.h"
#include "SpaghettiEng/Resource/ResourceManager.h"
#include "SpaghettiEng/Scene/Transform.h"
#include "SpaghettiEng/Scene/Components.h"
#include "SpaghettiEng/Render/Mesh/Mesh.h"
#include "SpaghettiEng/Render/Mesh/Material.h"
#include "SpaghettiEng/Render/Camera/Camera.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"

// {} [] 
namespace Spg
{
  
  Scene& SceneManager::CreateScene(const std::string& name)
  {
    SPG_ASSERT(!m_scenes.contains(name));
    m_scenes[name] = Scene();
    m_active_sim_scene_name = name;
    return GetActiveScene();
  }

  void SceneManager::SetActiveScene(const std::string& name)
  {
    SPG_ASSERT(m_scenes.contains(name));
    m_active_sim_scene_name = name;
  }

  Scene& SceneManager::GetActiveScene()
  {
    auto it = m_scenes.find(m_active_sim_scene_name);
    SPG_ASSERT(it != m_scenes.end());
    Scene& scene = it->second;
    return scene;
  }

  void SceneManager::BuildDefaultScene(const ResourceManager& resource_manager)
  {
    Scene& scene = CreateScene("DefaultScene");
    auto& scene_reg = scene.GetRegistry();

    auto& mesh_cache = resource_manager.GetResourceCache<Mesh>();
    auto& shader_cache = resource_manager.GetResourceCache<GLShader>();

    auto grid_mesh_id = mesh_cache.GetResourceID("grid");
    auto coords_mesh_id = mesh_cache.GetResourceID("coords");
    auto shader_id = shader_cache.GetResourceID("Basic Shader");

    Entity grid_entity = scene_reg.CreateEntity();
    scene_reg.AddComponent<TagComponent>(grid_entity, TagComponent{"Grid"});
    scene_reg.AddComponent<Transform>(grid_entity);
    scene_reg.AddComponent<MeshHandle>(grid_entity, grid_mesh_id);
    scene_reg.AddComponent<Material>(grid_entity, shader_id);

    Entity coords_entity = scene_reg.CreateEntity();
    scene_reg.AddComponent<TagComponent>(coords_entity, TagComponent{"Coords"});
    scene_reg.AddComponent<Transform>(coords_entity);
    scene_reg.AddComponent<MeshHandle>(coords_entity, coords_mesh_id);
    scene_reg.AddComponent<Material>(coords_entity, shader_id);


    // Entity camera_entity = scene.GetRegistry().CreateEntity();
    // scene.GetRegistry().AddComponent<TagComponent>(camera_entity, TagComponent{"Scene Camera"});
    // scene.GetRegistry().AddComponent<Transform>(camera_entity);
    // scene.GetRegistry().AddComponent<Camera>(camera_entity);
    
  }

 

}