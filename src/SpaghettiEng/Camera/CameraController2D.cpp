#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CameraController2D.h"

namespace Spg
{
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