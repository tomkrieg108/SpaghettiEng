#pragma once

//#include <functional> //std::hash
#include <cstdint> // uint32_t
#include <vector>
#include <unordered_map>
#include <string>
//#include <utility> //std::forward
//#include <concepts>

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
    //static_assert(std::derived_from<T, ResourceBase<T>>, "T must derive from ResourceBase<T>");

  public:

    ResourceID<T> Add(T&& resource, const std::string& name)
    {
      auto it = m_name_to_id.find(name);
      if (it != m_name_to_id.end())
      {
        SPG_WARN("Resource name: {} in cache for: {} already exists.  Not added ", 
          name, typeid(T).name());
        return it->second;
      } 

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
    }

    const T& Get(ResourceID<T> id) const
    {
      //* Note: If m_resources reallocates, every T& handed out is invalidated
      SPG_ASSERT(id.index < m_resources.size() && m_generations[id.index] == id.generation);
      return m_resources[id.index];
    }

    T& Get(ResourceID<T> id)
    {
      SPG_ASSERT(id.index < m_resources.size() && m_generations[id.index] == id.generation);
      return m_resources[id.index];
    }
    
    T* GetPtr(ResourceID<T> id)
    {
      //* Note If m_resources reallocates, every T* previously handed out is invalidated
      if (id.index >= m_resources.size() || m_generations[id.index] != id.generation)
        return nullptr;   // stale or invalid handle — caught, not silently wrong
      return &m_resources[id.index];
    }

    void Remove(ResourceID<T> id)
    {
      if (id.index >= m_resources.size() || m_generations[id.index] != id.generation)
        return;
      m_generations[id.index]++; // invalidates every existing handle to this slot
      m_free_list.push_back(id.index);
    }

    ResourceID<T> GetResourceID(const std::string& name) const {
      auto it = m_name_to_id.find(name);
      SPG_ASSERT(it != m_name_to_id.end());
      return it->second; 
    }

    auto begin() { return m_resources.begin(); }
    auto end() { return m_resources.end(); }
    auto begin() const { return m_resources.cbegin(); }
    auto end()	const { return m_resources.cend(); }

    private:
      std::vector<T> m_resources;
      std::vector<uint32_t> m_generations;
      std::vector<uint32_t> m_free_list;
      std::unordered_map<std::string, ResourceID<T>> m_name_to_id;
      
  };

}





