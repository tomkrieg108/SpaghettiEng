#include "Physics/Core.h"
#include <Common/Common.h>

#include "Math/MathX.h"

namespace Phys
{
  void HelloPhysLib() {
    SPG_WARN("Hello from Physics Lib!");
    MathX::HelloMathLib();
  }
}