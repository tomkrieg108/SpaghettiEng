#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Core/Layer.h"


//For now this is just a static class rather than a layer
namespace Spg
{
  class Window;

  class ImGuiLayer
  {
    public:
      static void Initialise(const Window& window);
      static void PreRender();
      static void PostRender();
      static void Shutdown();

      static bool WantCaptureMouse();
      static bool WantCaptureKeyboard();
  };

  class ImGuiLayer2 : public Layer
  {
    public:
      void OnAttach() override {};
      void OnDetach() override {};

      static void Initialise(const Window& window) {};
      static void PreRender() {};
      static void PostRender() {};
      static void Shutdown() {};

      static bool WantCaptureMouse() {};
      static bool WantCaptureKeyboard() {};

  };
}