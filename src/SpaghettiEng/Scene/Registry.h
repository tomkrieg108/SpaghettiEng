#pragma once

//* NOTE: avoid including this in too many other header files

#include <cstdint>
#include <memory>

#include <entt/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "SpaghettiEng/Scene/Entity.h"


namespace Spg
{
  class Registry
  {
    public:

      ~Registry() = default;

      Entity CreateEntity()
      {
        Entity entity;
        entity.handle = m_registry.create(); 
        return entity;
      }
      
      void DestroyEntity(Entity entity)
      {
        m_registry.destroy(entity.handle);
      }

      bool IsValid(Entity entity) const 
      { 
        return m_registry.valid(entity.handle); 
      }

      template <typename T, typename... Args>
      T& AddComponent(Entity entity, Args&&... args)
      {
          T& component = m_registry.emplace<T>(entity.handle, std::forward<Args>(args)...);
          return component;
      }

      template <typename T, typename... Args>
      T& AddOrReplaceComponent(Entity entity, Args&&... args)
      {
        T& component = m_registry.emplace_or_replace<T>(entity.handle, std::forward<Args>(args)...);
        return component;
      }

      template <typename T>
      bool HasComponent(Entity entity) const
      {
          return m_registry.any_of<T>(entity.handle);
      }

      template <typename T>
      T& GetComponent(Entity entity)
      {
        return m_registry.get<T>(entity.handle);
      }

      template <typename T>
      const T& GetComponent(Entity entity) const
      {
        return m_registry.get<T>(entity.handle);
      }

      template <typename T>
      T* TryGetComponent(Entity entity) const
      {
        return m_registry.try_get<T>(entity.handle);
      }

      template <typename T>
      void RemoveComponent(Entity entity)
      {
          m_registry.remove<T>(entity.handle);
      }
      
    private:
    
      entt::registry m_registry;
  };
  

}

//AI
/*

To understand why your current setup causes a bottleneck, we have to look at how C++ handles templates.Because AddComponent<T> and GetComponent<T> are templates, the compiler cannot generate their machine code until it knows what T is (e.g., TransformComponent). Therefore, the source code for EnTT's internal mechanics must be visible to every .cpp file that uses your wrapper.If you include  in your Registry.h, every single file that includes Registry.h is forced to parse all of EnTT's registry code, even if that file only needs to call CreateEntity() and doesn't touch components at all.🛠️ The Solution: Splitting into .h and .inline.hBy splitting your wrapper, you isolate the heavy EnTT template dependencies.Registry.h: Contains only the class definition and forward declarations. It is extremely lightweight. Files that only create or destroy entities include only this.Registry.inline.h: Contains the heavy template definitions. Only files that actually add, remove, or modify components include this file.Here is how you implement this pattern:

1. The Clean Header File (Registry.h)

This file is stripped of the heavy EnTT registry include. It uses <entt/fwd.hpp> to keep compilation incredibly fast.

#pragma once

#include <cstdint>
#include <memory>
#include <entt/fwd.hpp> // Lightweight forward declarations
#include "SpaghettiEng/Scene/Entity.h"

namespace Spg
{
  class Registry
  {
    public:
      // We must explicitly declare the constructor and destructor here,
      // but define them in the .cpp file so the compiler doesn't need 
      // the full entt::registry definition right now.
      Registry();
      ~Registry();

      Entity CreateEntity();
      void DestroyEntity(Entity entity);
      bool IsValid(Entity entity) const;

      // Template declarations (No code bodies here!)
      template <typename T, typename... Args>
      T& AddComponent(Entity entity, Args&&... args);

      template <typename T, typename... Args>
      T& AddOrReplaceComponent(Entity entity, Args&&... args);

      template <typename T>
      bool HasComponent(Entity entity) const;

      template <typename T>
      T& GetComponent(Entity entity);

      template <typename T>
      const T& GetComponent(Entity entity) const;

      template <typename T>*
      TryGetComponent(Entity entity) const;

      template <typename T>
      void RemoveComponent(Entity entity);
      
    private:
      // This requires the full definition of entt::registry, 
      // which is why we must define the constructor/destructor in the .cpp
      entt::registry m_registry; 
  };
}

2. The Inline Header File (Registry.inline.h)

This file houses the actual template implementations and pulls in the heavy EnTT headers.

#pragma once

// Pull in the main header first
#include "Registry.h" 

// Now pull in the heavy EnTT dependency safely isolated here
#include  
#include <utility>

namespace Spg
{
  template <typename T, typename... Args>
  T& Registry::AddComponent(Entity entity, Args&&... args)
  {
      return m_registry.emplace<T>(entity.handle, std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  T& Registry::AddOrReplaceComponent(Entity entity, Args&&... args)
  {
    return m_registry.emplace_or_replace<T>(entity.handle, std::forward<Args>(args)...);
  }

  template <typename T>
  bool Registry::HasComponent(Entity entity) const
  {
      return m_registry.any_of<T>(entity.handle);
  }

  template <typename T>
  T& Registry::GetComponent(Entity entity)
  {
    return m_registry.get<T>(entity.handle);
  }

  template <typename T>
  const T& Registry::GetComponent(Entity entity) const
  {
    return m_registry.get<T>(entity.handle);
  }

  template <typename T>
  T* Registry::TryGetComponent(Entity entity) const
  {
    return m_registry.try_get<T>(entity.handle);
  }

  template <typename T>
  void Registry::RemoveComponent(Entity entity)
  {
      m_registry.remove<T>(entity.handle);
  }
}

3. The Source File (Registry.cpp)
This handles the non-templated function bodies.

#include "Registry.h"
#include  // Needed for constructor/destructor layout

namespace Spg
{
  Registry::Registry() = default;
  Registry::~Registry() = default;

  Entity Registry::CreateEntity()
  {
    Entity entity;
    entity.handle = m_registry.create(); 
    return entity;
  }
  
  void Registry::DestroyEntity(Entity entity)
  {
    m_registry.destroy(entity.handle);
  }

  bool Registry::IsValid(Entity entity) const 
  { 
    return m_registry.valid(entity.handle); 
  }
}

How to use this in your EngineInside SceneManager.cpp or basic engine systems:If a system only manages scene loading, creation, or deletion, just #include "Registry.h". It compiles instantly.Inside Gameplay Systems (e.g., PhysicsSystem.cpp, RenderSystem.cpp):Because these files actively get and add components, they will #include "Registry.inline.h".This completely protects your core engine loop from unnecessary compilation overhead.
*/