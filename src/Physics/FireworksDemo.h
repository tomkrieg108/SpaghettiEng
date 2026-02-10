#pragma once

#include "Math/MathX.h"
#include "Physics/Particle.h"

namespace Phys
{
  class FireworksDemo
  {
    public:
      void OnUpdate(MathX::Real time_step);
      void Initialise();

    private:
  };
}