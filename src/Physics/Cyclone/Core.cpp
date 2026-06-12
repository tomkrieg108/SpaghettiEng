#include "Physics/Cyclone/Core.h"
#include <Core/Common.h>

#include "Math/MathX.h"

namespace Cyc
{
  void HelloCycloneLib() {
    SPG_WARN("Hello from Cyclone Physics Lib!");
    MathX::HelloMathLib();
  }
}