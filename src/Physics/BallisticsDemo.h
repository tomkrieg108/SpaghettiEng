#pragma once

#include "Math/MathX.h"
#include "Physics/Particle.h"


namespace Phys
{
  class BallisticsDemo
  {
    public:

      void OnUpdate(MathX::Real time_step);
      void SetParameters();


    private:
      Particle m_particle;

  };
}