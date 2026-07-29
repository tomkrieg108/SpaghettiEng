#pragma once

#include <cstdint> // 
#include <string>
#include <limits> // std::numeric_limits
/*
  {} [] 
*/

namespace Spg
{
  template<typename T>
  struct ResourceID
  {
    uint32_t index = InvalidIndex;
    uint32_t generation = 0;

    static constexpr uint32_t InvalidIndex = std::numeric_limits<uint32_t>::max();
    explicit operator bool() const { return index != InvalidIndex; }
    bool IsValid() const { return index != InvalidIndex; }

    //auto-generates all comparison operators <, <= etc
    auto operator<=>(const ResourceID&) const = default; 
  };

  // =====================================================
  // Currently not used - to use, every resource needs to inherit from this as below
  template<typename T>
  struct ResourceBase
  {
    ResourceID<T> id{};
    std::string name;

    template<typename U> friend class ResourceCache;
  };
  struct SomeResource : ResourceBase<SomeResource> {};
  // =====================================================

}