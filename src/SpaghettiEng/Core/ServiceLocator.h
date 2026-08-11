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