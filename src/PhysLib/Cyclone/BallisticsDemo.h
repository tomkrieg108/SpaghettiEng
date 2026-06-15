#pragma once

#include "MathLib/MathLib.h"
#include "PhysLib/Cyclone/Particle.h"


namespace Cyc
{
  class BallisticsDemo
  {
    public:

      void OnUpdate(SpgMth::Real time_step);
      void SetParameters();


    private:
      Particle m_particle;

  };
}