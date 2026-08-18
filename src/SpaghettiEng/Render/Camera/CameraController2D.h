#pragma once

//#include "Camera2D.h"
#include "SpaghettiEng/Core/WindowEvents.h"

// {} [] 
namespace Spg
{
   class Camera2D;
  // class Camera;
  // class Transform;
  // class InputState;
  // class Window;

#if 0
  class TransformController
  {
    public:

    private:

  };

  class CameraController
  {
    public:
      
      void Update(double delta_time, const Window& window, 
                  Transform& transform);
      void OnEvent(WinEvt::Event& event,  const Window& window, 
                  Transform& transform, Camera& camera);


    private:
      void OnWindowResize(WinEvt::WindowResize& e);
      void OnMouseMoved(WinEvt::MouseMoved& e);
      void OnMouseScrolled(WinEvt::MouseScrolled& e);
      void OnMouseButtonPressed(WinEvt::MouseBtnPressed& e); 
  };
#endif  

  class CameraController2D
  {
    public:
      CameraController2D(Camera2D& camera);
      ~CameraController2D() = default;

      void Pan(float deltaX, float deltaY);
      void Zoom(float zoom);
      void Rotate(float degrees);

      Camera2D& GetCamera() {return m_camera;}

    private:
      Camera2D& m_camera;
  };
}