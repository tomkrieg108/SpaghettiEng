#pragma once

#include <cstdint>
#include <memory>

#include <entt/entt.hpp> 


namespace Spg
{
 
  struct Entity
  {
      operator entt::entity() const{ return handle; }
      operator std::uint32_t() const { return entt::to_integral(handle); }
      operator bool() const { return handle != entt::null; }

      entt::entity handle;
  };

  
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
      void RemoveComponent(Entity entity)
      {
          m_registry.remove<T>(entity.handle);
      }
      
    private:
    
      entt::registry m_registry;
  };
  

}