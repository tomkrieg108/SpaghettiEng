#pragma once

#include "Math/MathX.h"
#include "Physics/Cyclone/Particle.h"

namespace Cyc
{
  class FireworksDemo
  {
    public:
      void OnUpdate(MathX::Real time_step);
      void Initialise();

    private:
  };
}