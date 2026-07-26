#pragma once

//#include <functional> //std::hash
#include <cstdint> // uint32_t

#include <vector>
#include <unordered_map>
#include <string>
#include <utility> //std::forward
#include <concepts>

#include "SpaghettiEng/Resource/Resource.h"
#include "CoreLib/Core.h" 

/*
  {} []
*/

namespace Spg
{

  template<typename T>
  class ResourceCache
  {
    static_assert(std::derived_from<T, ResourceBase<T>>, "T must derive from ResourceBase<T>");

  public:

    ResourceID<T> Add(T&& resource, const std::string& name)
    {
      uint32_t idx;
      if (!m_free_list.empty())
      {
        idx = m_free_list.back();
        m_free_list.pop_back();
        m_resources[idx] = std::move(resource);
      }
      else
      {
        idx = static_cast<uint32_t>(m_resources.size());
        m_resources.push_back(std::move(resource));
        m_generations.push_back(0);
      }

      ResourceID<T> id{ idx, m_generations[idx] };
      m_name_to_id[name] = id;
      return id;

      #if 0
      auto it = m_name_to_id.find(resource_name);
      if (it != m_name_to_id.end())
      {
        SPG_WARN("Resource: {} in cache for: {} already exists.  Not added ", resource_name, typeid(T).name());
        return it->second;
      } 

      ResourceID id = static_cast<ResourceID>(m_resources.size());
     
      resource.id = id;
      resource.name = resource_name;

      m_resources.push_back(std::move(resource));
      m_name_to_id[resource_name] = id;
      return id;
    #endif  
    }

    #if 1
    const T& Get(ResourceID<T> id)
    {
      // Note: If m_resources reallocates, every T& handed out is invalidated
      SPG_ASSERT(id < m_resources.size() && m_generations[id.index] == id.generation);
      return m_resources[id];
    }
    #endif

    T* GetPtr(ResourceID<T> id)
    {
      // Note If m_resources reallocates, every T* previously handed out is invalidated
      if (id.index >= m_resources.size() || m_generations[id.index] != id.generation)
        return nullptr;   // stale or invalid handle — caught, not silently wrong
      return &m_resources[id.index];
    }

    void Remove(ResourceID<T> id)
    {
      if (id.index >= m_resources.size() || m_generations[id.index] != id.generation)
        return;
      m_generations[id.index]++;      // invalidates every existing handle to this slot
      m_free_list.push_back(id.index);
    }

    private:
      std::vector<T> m_resources;
      std::vector<uint32_t> m_generations;
      std::vector<uint32_t> m_free_list;
      std::unordered_map<std::string, ResourceID<T>> m_name_to_id;
      
  };

}


//Store by value?
/*
Is Storing by Value Okay? 

Your reasoning for storing Mesh by value is completely valid. If your types are cheap to move (like std::vector), moving them into the map only copies a few pointers (24 bytes for a vector), not the heavy heap data.

However, there are still subtle reasons why std::unique_ptr wins in engine architecture, and a few ways to solve your naming collision problem.

(The Pointer Stability Trade-off)Yes, storing by value is functionally fine because std::unordered_map guarantees pointer stability. Node-based containers like std::unordered_map do not move elements in memory when the map resizes or rehashes. A pointer to a Mesh inside the map remains valid until that specific mesh is deleted.

However, std::unique_ptr<T> is still highly recommended for three reasons:

Polymorphism Support: If you ever want ResourceCache<Texture> to store a derived Texture2D or Cubemap, a value-based map will suffer from object slicing. A std::unique_ptr<Texture> allows polymorphism.

API Flexibility: Returning a raw pointer (T*) from a map of std::unique_ptr<T> clearly signals "the cache owns this, you just look at it." If you store by value, you have to return a pointer to the internal map node (&it->second), which can feel slightly less clean architecture-wise.

No-Throw Move Guarantees: For value-based storage to be fast, your resource types must have noexcept move constructors. If a custom resource lacks this, the map may fallback to expensive copies during internal operations.

Verdict: If your resources are strictly final structs (no inheritance) and have clean move semantics, storing by value is perfectly acceptable.
*/





