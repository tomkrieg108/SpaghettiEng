#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <cstdint>

#include "SpaghettiEng/Scene/Scene.h"
//#include "SpaghettiEng/Scene/Transform.h"
#include "SpaghettiEng/Math/Transform.h"
#include "SpaghettiEng/Render/Camera/Camera.h"


// {} [] 
namespace Spg
{
  class ResourceManager;

  class SceneManager
  {
    public:
      SceneManager() = default;
      ~SceneManager() = default;

      Scene& CreateScene(const std::string& name);
      void SetActiveScene(const std::string& name);

      Scene& GetActiveScene();
      void RenameScene(const std::string& name, const std::string& new_name) {}
      void LoadScene(const std::string& name) {} 
      void UnloadScene(const std::string& name) {} 
      
      uint32_t GetSceneCount() { return m_scenes.size(); }

      void BuildDefaultScene(const ResourceManager& resource_manager);
      
      Transform& GetSceneCameraTransform() {return m_scene_camera_transform;}
      Camera& GetSceneCamera() {return m_scene_camera;}

    private:
      //* Note: Getting active scene requires slowish map lookup (in the update loop) - 
      std::unordered_map<std::string, Scene> m_scenes;
      std::string m_active_sim_scene_name = std::string("");

      // Scene camera as instance member rather than entity
      Camera m_scene_camera;
      Transform m_scene_camera_transform;
      // Include a controller that operator on the transfom
  };

}