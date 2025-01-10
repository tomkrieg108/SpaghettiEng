#pragma once

#include "Camera2D.h"

namespace Spg
{
  class CameraController2D
  {
    public:
      CameraController2D(Camera2D& camera);
      ~CameraController2D() = default;

      void Pan(float deltaX, float deltaY);
      void Zoom(float zoom);
      void Rotate(float degrees);

    private:
      Camera2D& m_camera;
  };
}