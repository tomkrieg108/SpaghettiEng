#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//For now this is just a static class rather than a layer
namespace Spg
{
  class Window;

  class ImGuiContext
  {
    public:

    static void Initialise(const Window& window);
    static void PreRender();
    static void PostRender();
    static void Shutdown();

    static void SetDarkThemeColors();
    static bool WantCaptureMouse();
    static bool WantCaptureKeyboard();
    
  };
}