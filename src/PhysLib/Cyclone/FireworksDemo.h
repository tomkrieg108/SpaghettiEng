#pragma once

#include "MathLib/MathLib.h"
#include "PhysLib/Cyclone/Particle.h"

namespace Cyc
{
  class FireworksDemo
  {
    public:
      void OnUpdate(SpgMth::Real time_step);
      void Initialise();

    private:
  };
}