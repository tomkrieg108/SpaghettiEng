#pragma once


//For now this is just a static class rather than a layer
namespace Spg
{
  class Window;

  class ImGuiLayer
  {
    public:
      static void Initialise(const Window& window);
      //static void Initialise(Window* window);
      static void PreRender();
      static void PostRender();
      static void Shutdown();

      static bool WantCaptureMouse();
      static bool WantCaptureKeyboard();
  };
}