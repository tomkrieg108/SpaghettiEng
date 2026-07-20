#pragma once

// #include <unordered_map>
// #include <memory>
// #include <string>
// #include <cstdint>

#include "SpaghettiEng/Scene/SceneVisuals.h"

#include "SpaghettiEng/Scene/Registry.h"

// {}
namespace Spg
{
  //SceneVisuals::SceneVisuals() : m_registry(std::make_unique<Registry>()) {}
  //SceneVisuals::~SceneVisuals() = default;

  SceneVisuals::SceneVisuals() : m_registry(new (Registry)) {}
  SceneVisuals::~SceneVisuals()
  {
    if(m_registry != nullptr)
      delete m_registry;
  }

  SceneVisuals SceneVisuals::CreateDefault()
  {
    
  }

}