#pragma once

#include <filesystem>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <utility> //std::forward
//#include <concepts>

#include "SpaghettiEng/Resource/Resource.h"
#include "SpaghettiEng/Resource/ResourceCache.h"

/*
AI
Yes, a Meyers Singleton is highly preferred over a purely static class in C++ whenever your design requires managing object state, controlling initialization order, or leaving room for object-oriented flexibility. While purely static methods are useful for stateless collections of utility functions, a Meyers Singleton leverages C++ language guarantees to solve critical global life-cycle problems.

Why the Meyers Singleton Wins

1. Eliminates the Static Initialization Order FiascoIn C++, the initialization order of global/static variables defined in different files is undefined. If a static variable in FileA.cpp tries to use a static class method in FileB.cpp during startup, it may access uninitialized memory. “The Meyers Singleton is based on a local static,” which guarantees the object is safely constructed the very first time it is requested.

2. Standardized Thread-Safe Lazy InitializationSince C++11, the initialization of local block-scope statics is natively thread-safe. “According to the standard, static local variables are initialized in a thread-safe manner,” which means you get flawless lazy initialization without writing custom mutex locks. A static class holding static data members cannot natively guarantee this without verbose synchronization boilerplate.

3. Object-Oriented FlexibilityA Meyers Singleton deals with an actual object instance (Singleton&). This means your singleton can inherit from a base configuration class, implement an abstract interface, and be passed directly into functions accepting regular references. Pure static classes have no instances, making polymorphism impossible.

4. Clearer Path to Dependency InjectionBoth patterns introduce global state, which complicates testing. However, if you need to decouple your code later, changing a client from Singleton::getInstance().foo() to accept a reference foo(ISingleton& instance) is straightforward. Refactoring away from hardcoded static calls like StaticClass::foo() requires rewriting lines across your entire codebase.

When to Prefer a Static Class (or Namespace)A purely static class—or preferably a native C++ namespace—is superior only when you have no state to maintain. “For smaller utility functions like math functions or string manipulation, static is better.” If you do not need an object instance, lifetime management, or polymorphism, bundle your pure functions into a namespace

// USE A NAMESPACE for stateless utility libraries
namespace MathUtils {
    double square(double x) { return x * x; }
}

// USE A MEYERS SINGLETON for stateful or order-dependent systems
class DatabaseManager {
public:
    static DatabaseManager& getInstance() {
        static DatabaseManager instance; // Thread-safe lazy initialization
        return instance;
    }
    void query(const std::string& sql);

private:
    DatabaseManager() = default; // Prevent external instantiation
    ~DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete; // Disable copying
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};


*/

/*
  {} []
*/

namespace Spg
{
  namespace fs = std::filesystem;

  class ResourceManager
  {
  public:

    ResourceManager();
    void Init();
    fs::path GetAssetsPath()  { return m_assets_path; } 

    template<typename T, typename... Args>
      //requires std::derived_from<T, ResourceBase<T>>
    ResourceCache<T>& MakeResourceCache(Args&&... args) {
      Register<ResourceCache<T>>(std::forward<Args>(args)...);
      return Get<ResourceCache<T>>();
    }

    template<typename T>
    ResourceCache<T>& GetResourceCache() {
      return Get<ResourceCache<T>>();
    }

    template<typename T>
    const ResourceCache<T>& GetResourceCache() const {
      return Get<ResourceCache<T>>();
    }

    template<typename T>
    ResourceCache<T>* TryGetResourceCache() {
      return TryGet<ResourceCache<T>>();
    }

  private:
    void SetAssetsPath();

    fs::path SearchUp(fs::path current_dir, const std::string& target_directory);
    fs::path SearchDown(fs::path current_dir, const std::string& target_dir);
    fs::path GetExecutableDirectory();

  private:  
    fs::path m_assets_path;

  private:
 
    template<typename T, typename... Args>
    T& Register(Args&&... args) 
    {
      // Enforce that we only register a type once
      auto [it, inserted] = m_map.emplace(
        typeid(T),
        std::make_unique<TypeWrapper<T>>(std::forward<Args>(args)...)
      );
      if(!inserted)
          SPG_WARN("Resource: {} already exists ", typeid(T).name());
      
      return *static_cast<TypeWrapper<T>*>(it->second.get())->instance; 
    }

    template<typename T>
    T& Get() 
    {
        auto it = m_map.find(typeid(T));
        SPG_ASSERT(it != m_map.end());
        return *static_cast<TypeWrapper<T>*>(it->second.get())->instance;
    }

    template<typename T>
    const T& Get() const
    {
        auto it = m_map.find(typeid(T));
        SPG_ASSERT(it != m_map.end());
        return *static_cast<TypeWrapper<T>*>(it->second.get())->instance;
    }

    template<typename T>
    T* TryGet()
    {
      auto it = m_map.find(typeid(T));
      if (it == m_map.end())
          return nullptr;
      return *static_cast<TypeWrapper<T>*>(it->second.get())->instance;    
    }
    
    struct IBaseType 
    { 
      //Additional virtual hooks can be added
      virtual ~IBaseType() = default; 
    };

    template<typename T>
      struct TypeWrapper : public IBaseType {

          template<typename... Args>
          TypeWrapper(Args&&... args) :
            instance(std::make_unique<T>(std::forward<Args>(args)...))
          {} 
          
          std::unique_ptr<T> instance;
      };

      std::unordered_map<std::type_index, std::unique_ptr<IBaseType>> m_map;
  };
}