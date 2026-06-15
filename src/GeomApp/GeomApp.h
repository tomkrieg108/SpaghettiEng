#pragma once
#include <SpaghettiEng/SpaghettiEng.h>
#include "MathLib/MathLib.h"
#include <PhysLib/Cyclone/Core.h>
#include <Geometry/Init.h>
#include <cstdint>
#include <limits>
using namespace std::string_literals;

namespace Spg
{
  class GeomApp : public Application
  {
  public:
    GeomApp(const std::string& title);
    ~GeomApp();
  private:
  };

  void Init();
}
