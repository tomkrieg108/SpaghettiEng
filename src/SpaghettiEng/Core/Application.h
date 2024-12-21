#pragma once
#include <memory>

class Window;

namespace Spg
{
  class Application
  {
    public:
      Application() {}
      virtual ~Application() = default;

      void Initialise();
      void Run();
      void Shutdown();

    private:
      std::unique_ptr<Window> m_window = nullptr;
  };

  void EngLibHello();

}
