#pragma once
//#include <memory> //Todo - include in PCH

namespace Spg //todo everything else in Common uses Utils namespace
{
  enum class PtrType {Scope,Ref,Raw};

  template<typename T>
  using Ref = std::shared_ptr<T>;

  template<typename T>
  using Scope = std::unique_ptr<T>;
  
  template<typename T, typename ... Args>
  constexpr Ref<T> CreateRef(Args&& ... args)
  {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template<typename T, typename ... Args>
  constexpr Scope<T> CreateScope(Args&& ... args)
  {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
}