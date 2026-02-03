#pragma once
#include <SpaghettiEng/SpaghettiEng.h>
#include <Math/MathX.h>
#include <Physics/Core.h>
#include <Geometry/Geometry.h>
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

  void HelloGeomApp();
}
