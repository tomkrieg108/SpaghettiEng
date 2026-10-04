#include "SpaghettiEng/Scene/SceneManager.h"

#include <glm/vec3.hpp>

#include "CoreLib/Core.h"

#include "SpaghettiEng/Scene/Entity.h"
#include "SpaghettiEng/Scene/Registry.h"

//For BuildDefaultScene()

#include "SpaghettiEng/Resource/Resource.h"
#include "SpaghettiEng/Resource/ResourceCache.h"
#include "SpaghettiEng/Resource/ResourceManager.h"
//#include "SpaghettiEng/Scene/Transform.h"
#include "SpaghettiEng/Math/Transform.h"
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
    auto cube_mesh_id = mesh_cache.GetResourceID("cube");
    //auto sphere_mesh_id = mesh_cache.GetResourceID("sphere_tm");
    auto sphere_mesh_id = mesh_cache.GetResourceID("sphere");

    // auto shader_id = shader_cache.GetResourceID("Basic Shader");
    auto basic_shader_id = shader_cache.GetResourceID("Basic UB Shader");
    auto basic_lighting_shader_id = shader_cache.GetResourceID("Basic Lighting Shader");

    Entity grid_entity = scene_reg.CreateEntity();
    scene_reg.AddComponent<TagComponent>(grid_entity, TagComponent{"Grid"});
    scene_reg.AddComponent<Transform>(grid_entity);
    scene_reg.AddComponent<MeshHandle>(grid_entity, grid_mesh_id);
    scene_reg.AddComponent<Material>(grid_entity, Material{.shader_id = basic_shader_id});

    Entity coords_entity = scene_reg.CreateEntity();
    scene_reg.AddComponent<TagComponent>(coords_entity, TagComponent{"Coords"});
    scene_reg.AddComponent<Transform>(coords_entity);
    scene_reg.AddComponent<MeshHandle>(coords_entity, coords_mesh_id);
    scene_reg.AddComponent<Material>(coords_entity, basic_shader_id);

    // Entity cube_entity = scene_reg.CreateEntity();
    // scene_reg.AddComponent<TagComponent>(cube_entity, TagComponent{"Cube"});
    // scene_reg.AddComponent<Transform>(cube_entity);
    // scene_reg.AddComponent<MeshHandle>(cube_entity, cube_mesh_id);
    // scene_reg.AddComponent<Material>(cube_entity, basic_lighting_shader_id, glm::vec3(1,0,0));

    Entity sphere_entity = scene_reg.CreateEntity();
    scene_reg.AddComponent<TagComponent>(sphere_entity, TagComponent{"Sphere_tm"});
    scene_reg.AddComponent<Transform>(sphere_entity);
    scene_reg.AddComponent<MeshHandle>(sphere_entity, sphere_mesh_id);
    scene_reg.AddComponent<Material>(sphere_entity, basic_lighting_shader_id, glm::vec3(0,0.5,0));

    m_scene_camera_transform.SetPosition(glm::vec3(5,3,2));
    m_scene_camera_transform.LookAt(glm::vec3(0,0,0));
  }

}