
#include "SpaghettiEng/Render/Camera/CameraController2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SpaghettiEng/Core/KeyCodes.h"
#include "SpaghettiEng/Core/MouseCodes.h"
#include "SpaghettiEng/Core/InputState.h"
#include "SpaghettiEng/Core/WindowEvents.h"


#include "SpaghettiEng/Render/Camera/Camera2D.h"

// {} [] 
namespace Spg
{

#if 0
  void InputHandler::Update(double delta_time, const InputState& input_state, 
    Transform& transform)
  {

  }

  void InputHandler::OnEvent(WinEvt::Event& event, const InputState& input_state, 
    Transform& transform)
  {

  }
#endif

  CameraController2D::CameraController2D(Camera2D& camera) :
    m_camera{camera}
  {
  }

  void CameraController2D::Pan(float deltaX, float deltaY)
  {
    m_camera.m_transform = glm::translate(m_camera.m_transform, glm::vec3(deltaX,deltaY,0) );
  }

  void CameraController2D::Zoom(float zoom)
  {
    m_camera.m_transform = glm::scale(m_camera.m_transform, glm::vec3(zoom,zoom,0));
  }

  void CameraController2D::Rotate(float degrees)
  {
    float radians = glm::radians(degrees);
    m_camera.m_transform = glm::rotate(m_camera.m_transform, radians, glm::vec3(0,0,1));
  }

}