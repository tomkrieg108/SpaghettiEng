#pragma once
#include "CoreLib/Core.h"

#include <typeinfo>
#include <typeindex>
#include <memory>
#include <utility> //std::forward

/*
  {} []
*/

namespace Spg
{

  class ServiceLocator
  {
  public:
      
      /*
        typeid(T) executes: This produces an expression of type const std::type_info&

        Implicit match: The compiler looks at the return type, std::type_index

        Constructor call: The compiler finds the constructor std::type_index(const std::type_info& rhs).

        Conversion: The compiler automatically wraps the type_info inside a new std::type_index object and returns it.

        std::type_index has a constructor that takes a const std::type_info&
      */
      template<typename T>
      static std::type_index TypeKey()
      {
        // return std::type_index(typeid(T)); equivalent to below
        return typeid(T);
      }
  
      template<typename T, typename... Args>
      void Register(Args&&... args) 
      {
          // Enforce that we only register a type once
          auto [it, inserted] = m_services.emplace(
            typeid(T),
            std::make_unique<ServiceWrapper<T>>(std::forward<Args>(args)...)
          );
          if(!inserted)
            SPG_WARN("Service: {} already existis ", typeid(T).name());
      }

      template<typename T>
      T& Get() 
      {
        auto it = m_services.find(typeid(T));
        SPG_ASSERT(it != m_services.end());
        return *static_cast<ServiceWrapper<T>*>(it->second.get())->instance;
      }

      template<typename T>
      const T& Get() const
      {
        auto it = m_services.find(typeid(T));
        SPG_ASSERT(it != m_services.end());
        return *static_cast<ServiceWrapper<T>*>(it->second.get())->instance;
      }

      template<typename T>
      T* TryGet()
      {
        auto it = m_services.find(typeid(T));
        if (it == m_services.end())
            return nullptr;
        return static_cast<ServiceWrapper<T>*>(it->second.get())->instance.get();    
      }

  private:

      struct WrapperBase { virtual ~WrapperBase() = default; };
      
      template<typename T>
      struct ServiceWrapper : public WrapperBase {

          template<typename... Args>
          ServiceWrapper(Args&&... args) :
            instance(std::make_unique<T>(std::forward<Args>(args)...))
          {} 
          
          //todo: does instance also need to be a unique_ptr? why not an instance?
          std::unique_ptr<T> instance; 
      };

      std::unordered_map<std::type_index, std::unique_ptr<WrapperBase>> m_services;
  };
  
}

//AI
/*

Why Managers belong in the Service Locator

Putting your managers in the locator instead of making them individual singletons provides massive structural advantages:

Explicit Lifecycles: Your ResourceManager or AudioManager might take a long time to load or require clean hardware teardowns. Storing them here means their allocation and destruction happen predictably when the locator itself is initialized or destroyed.

Controlled Access: A singleton allows any code file in the entire project to casually load an asset or play a sound. By using a locator, a low-level math utility cannot accidentally call the AudioManager because it simply doesn't have a reference to the locator.

Zero Global Pollution: You maintain a single entry point for all sub-systems, which keeps your codebase organized.The Hidden Trap: "Service Locator Pattern" vs "Dependency Injection"While this approach is completely reasonable, using a Service Locator introduces a subtle design trade-off that you should watch out for as your engine grows: it hides dependencies.

The Hidden Trap: "Service Locator Pattern" vs "Dependency Injection"While this approach is completely reasonable, using a Service Locator introduces a subtle design trade-off that you should watch out for as your engine grows: it hides dependencies.

The ProblemIf a high-level module (like a GameWorld class) takes the entire ServiceLocator as a parameter, it looks like this:

void GameWorld::Update(ServiceLocator& locator) {
    // Hidden internal dependencies:
    auto& audio = locator.Get<AudioManager>();
    auto& scene = locator.Get<SceneManager>();
    
    audio.PlayAmbientSound();
}

Looking at the function signature Update(ServiceLocator& locator), an outside developer has no idea what GameWorld actually needs to run. It could be using one manager, or it could be using all fifty. This makes the class harder to isolate and unit test.

The Fix: "Locate at the Boundary"To get the best of both worlds, use the Service Locator at the highest level possible (the "boundary" of your module) to pull out the exact managers you need. Then, pass only those specific references down to the actual working classes.

// 1. High-level module extracts exactly what is needed at the boundary
void GameSystem::Initialize(ServiceLocator& locator) {
    m_audioManager = &locator.Get<AudioManager>();
    m_sceneManager = &locator.Get<SceneManager>();
}

// 2. Internal logic remains clean, explicit, and easy to test
void GameSystem::Update() {
    // No locator needed here anymore. Clear, explicit dependencies!
    m_audioManager->PlayAmbientSound();
}

Your architecture is in an excellent spot to manage a complex system cleanly. If you'd like to dive deeper, let me know. Would you like to look at how to handle cross-dependencies (e.g., if SceneManager itself needs the ResourceManager inside the locator), see how to create read-only vs mutable views of your services, or implement a TryGet<T>() method that safely returns a pointer instead of asserting?

*/

//AI modification

#if 0

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <cassert>

class ServiceLocator {
public:
    ServiceLocator() = default;
    ~ServiceLocator() = default;

    // Disallow copying to protect unique ownership of services
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    // InterfaceT: The type used for lookup (can be an interface or concrete class)
    // ConcreteT: The actual class being instantiated (defaults to InterfaceT)
    template<typename InterfaceT, typename ConcreteT = InterfaceT, typename... Args>
    void Register(Args&&... args) {
        std::type_index typeIdx = std::type_index(typeid(InterfaceT));
        
        // Enforce that we only register a type once
        assert(m_services.find(typeIdx) == m_services.end() && "Service already registered!");
        
        // Flattened: ServiceWrapper directly owns the concrete object on the heap
        m_services[typeIdx] = std::make_unique<ServiceWrapper<InterfaceT>>(
            std::make_unique<ConcreteT>(std::forward<Args>(args)...)
        );
    }

    template<typename InterfaceT>
    InterfaceT& Get() {
        std::type_index typeIdx = std::type_index(typeid(InterfaceT));
        auto it = m_services.find(typeIdx);
        
        assert(it != m_services.end() && "Requested service was never registered!");
        
        // Safely downcast the wrapper and return the reference
        auto* wrapper = static_cast<ServiceWrapper<InterfaceT>*>(it->second.get());
        return *(wrapper->instance);
    }

private:
    struct WrapperBase { 
        virtual ~WrapperBase() = default; 
    };
    
    template<typename InterfaceT>
    struct ServiceWrapper : public WrapperBase {
        std::unique_ptr<InterfaceT> instance;
        ServiceWrapper(std::unique_ptr<InterfaceT> inst) : instance(std::move(inst)) {}
    };

    std::unordered_map<std::type_index, std::unique_ptr<WrapperBase>> m_services;

#endif