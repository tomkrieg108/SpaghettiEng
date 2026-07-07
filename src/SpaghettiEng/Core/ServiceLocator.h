#pragma once
#include "CoreLib/Core.h"
#include <typeinfo>
#include <typeindex>
#include <memory>

namespace Spg
{

  class ServiceLocator //Service Locator
  {
  public:
      #define TYPE_IDX(T) std::type_index(typeid(T)) //preferred
      //#define TYPE_IDX(T) typeid(T).hash_code() 

      template<typename T, typename... Args>
      void Register(Args&&... args) 
      {
          // Enforce that we only register a type once
          SPG_ASSERT(m_services.find(TYPE_IDX(T)) == m_services.end());
          
          m_services[TYPE_IDX(T)] = std::make_unique<ServiceWrapper<T>>(std::make_unique<T>(std::forward<Args>(args)...));
      }

      template<typename T>
      T& Get() 
      {
          auto it = m_services.find(TYPE_IDX(T));
          SPG_ASSERT(it != m_services.end());
          return *static_cast<ServiceWrapper<T>*>(it->second.get())->instance;
      }

  private:

      struct WrapperBase { virtual ~WrapperBase() = default; };
      
      template<typename T>
      struct ServiceWrapper : public WrapperBase {
          std::unique_ptr<T> instance;
          ServiceWrapper(std::unique_ptr<T> inst) : instance(std::move(inst)) {}
      };

      std::unordered_map<std::type_index, std::unique_ptr<WrapperBase>> m_services;
  };
  
}