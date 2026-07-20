#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <cstdint>

#include "SpaghettiEng/Scene/Scene.h"

// {}
namespace Spg
{
  class SceneManager
  {
    public:
      SceneManager() = default;
      ~SceneManager() = default;

      void CreateScene(const std::string& name);
      void SetActiveScene(const std::string& name);
      Scene& GetActiveScene();
      void RenameScene(const std::string& name, const std::string& new_name) {}

      void LoadScene(const std::string& name) {} 
      void UnloadScene(const std::string& name) {} 
      
      uint32_t GetSceneCount() { return m_sim_scenes.size(); }

    private:
      std::unordered_map<std::string, Scene> m_sim_scenes;
      std::string m_active_sim_scene_name = std::string("");
  };

}