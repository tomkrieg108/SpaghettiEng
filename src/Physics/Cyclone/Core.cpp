#include "Physics/Cyclone/Core.h"
#include <Common/Common.h>

#include "Math/MathX.h"

namespace Cyc
{
  void HelloCycloneLib() {
    SPG_WARN("Hello from Cyclone Physics Lib!");
    MathX::HelloMathLib();
  }
}