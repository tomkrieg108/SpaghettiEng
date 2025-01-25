#pragma once
#include <Common/Common.h>
#include <any> //for V3!

namespace Spg
{

  namespace Ctx_V2
   {

    // template <typename T>
    // auto deleter = [](void* ptr) 
    // {
    //   delete static_cast<T*>(ptr);
    // }   

    template <typename T>
    static void DefaultDeleter(void* ptr)
    {
        delete static_cast<T*>(ptr);
    }
   
    class AppContext
    {
       using DeleterType = void(*)(void*);
    public:
      template <typename T>
      void Set(const std::string& name, Scope<T> value)
      {
        static_assert(!std::is_pointer_v<T>, "Do not pass raw pointers to AppComponent. Use std::unique_ptr instead.");
        auto it = m_data.find(name);
        SPG_ASSERT(it == m_data.end());
        // auto deleter = [](void* ptr) 
        // { 
        //   //Without this, would try to call delete on a void pointer
        //   delete static_cast<T*>(ptr); 
        // };

        // Cast the lambda to a proper function pointer
        //void(*deleter)(void*) = [](void* ptr) { delete static_cast<T*>(ptr); };
        DeleterType deleter = [](void* ptr) { delete static_cast<T*>(ptr); };
        
        // Cast the pointer to void* and assign it to unique_ptr<void, DeleterType>
        void* rawPtr = static_cast<void*>(value.release());
        //m_data[name] = std::unique_ptr<void, DeleterType>(rawPtr, deleter);
        //m_data[name] = std::unique_ptr<void, DeleterType>(rawPtr, &DefaultDeleter<T>);
        //m_data[name] = std::unique_ptr<void, void(*)(void*)>(rawPtr, &DefaultDeleter<T>);
        m_data[name] = std::unique_ptr<void, void(*)(void*)>(rawPtr, deleter);

        
        //m_data[name] = std::move(value);
      }

      template <typename T>
      T& Get(const std::string& name) const 
      {
        auto it = m_data.find(name);
        SPG_ASSERT(it != m_data.end());
        return *(static_cast<T*>(it->second.get()));
      }

    private:   
      std::unordered_map<std::string, std::unique_ptr<void, DeleterType>> m_data;
    };

   }  

  inline namespace Ctx_V3
  {
    class AppContext
    {
       
    public:
      template <typename T>
      void Set(const std::string& name, Ref<T> value)
      {
        //For'Ref<T> value' lvalues passed by value, rvalues passed by move
        static_assert(!std::is_pointer_v<T>, "Do not pass raw pointers to AppComponent. Use std::unique_ptr instead.");
        auto it = m_data.find(name);
        SPG_ASSERT(it == m_data.end());
        m_data[name] = std::move(value);
      }

      template <typename T>
      T& Get(const std::string& name) 
      {
        auto it = m_data.find(name);
        SPG_ASSERT(it != m_data.end());
        return *(static_cast<T*>(it->second.get()));
      }

      template <typename T>
      const T& Get(const std::string& name) const
      {
        return Get<T>(name);
      }

    private:   
      std::unordered_map<std::string, Ref<void>> m_data;
    };
  }

}