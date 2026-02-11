#pragma once

#include "Math/MathX.h"
#include "Physics/Cyclone/Particle.h"


namespace Cyc
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