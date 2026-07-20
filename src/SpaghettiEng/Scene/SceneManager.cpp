#include "SpaghettiEng/Scene/SceneManager.h"

#include <memory>

#include "CoreLib/Core.h"

// {} [] 
namespace Spg
{
  
  void SceneManager::CreateScene(const std::string& name)
  {
    SPG_ASSERT(!m_sim_scenes.contains(name));
    m_sim_scenes[name] = Scene();
    m_active_sim_scene_name = name;
  }

  void SceneManager::SetActiveScene(const std::string& name)
  {
    SPG_ASSERT(m_sim_scenes.contains(name));
    m_active_sim_scene_name = name;
  }

  Scene& SceneManager::GetActiveScene()
  {
    auto it = m_sim_scenes.find(m_active_sim_scene_name);
    SPG_ASSERT(it != m_sim_scenes.end());
    Scene& scene = it->second;
    return scene;
  }
}