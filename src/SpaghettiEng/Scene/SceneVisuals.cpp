#pragma once

#include "SpaghettiEng/Scene/SceneVisuals.h"

#include "SpaghettiEng/Scene/Registry.h"
#include "SpaghettiEng/Scene/Components.h"
#include "SpaghettiEng/Render/Mesh/Mesh.h"
#include "SpaghettiEng/Render/Mesh/MeshCache.h"
#include "SpaghettiEng/Render/Camera/Camera.h"

// {}
namespace Spg
{

  #if 1
  
  SceneVisuals::SceneVisuals() 
  : m_registry(new Registry() ) 
  {
    //Default visual elements

     // Grid:
    Entity grid_entity = m_registry->CreateEntity();
    m_registry->AddComponent<TagComponent>(grid_entity, TagComponent{"Grid"});
    m_registry->AddComponent<StaticTransform>(grid_entity);
    m_registry->AddComponent<MeshComponent>(grid_entity,MeshCache::GetMeshComponent("grid"));


    // World Coords
    Entity coords_entity = m_registry->CreateEntity();
    m_registry->AddComponent<TagComponent>(coords_entity, TagComponent{"Coords"});
    m_registry->AddComponent<StaticTransform>(coords_entity);
    m_registry->AddComponent<MeshComponent>(coords_entity,MeshCache::GetMeshComponent("coords"));

    // Scene camera 
    Entity camera_entity = m_registry->CreateEntity();
    m_registry->AddComponent<TagComponent>(camera_entity, TagComponent{"Scene Camera"});
    m_registry->AddComponent<StaticTransform>(camera_entity);
    m_registry->AddComponent<Camera>(camera_entity);
    
    auto& camera_name = m_registry->GetComponent<TagComponent>(camera_entity);
    auto& camera_tr = m_registry->GetComponent<StaticTransform>(camera_entity);
    auto& camera = m_registry->GetComponent<Camera>(camera_entity);
  }

  SceneVisuals::~SceneVisuals()
  {
    if(m_registry != nullptr)
      delete m_registry;
  }
  #endif
}