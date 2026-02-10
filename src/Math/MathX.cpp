#include "Math/MathX.h"
#include <Common/Common.h>

namespace MathX
{
  void HelloMathLib() {
    SPG_WARN("Hello from Maths Lib!");
  }

  Circle ComputeCircumCircle(Point2& a, Point2& b, Point2& c) {
    Circle out;
    out.center = {0,0};
    out.radius = -1.0; //Indicates invalid   

    Real ax = a.x, ay = a.y;
    Real bx = b.x, by = b.y;
    Real cx = c.x, cy = c.y;

    Real d = 2.0 * (
        ax * (by - cy) +
        bx * (cy - ay) +
        cx * (ay - by)
    );

    if (std::abs(d) < 1e-6)
        return out; // collinear or nearly so

    Real ax2ay2 = ax*ax + ay*ay;
    Real bx2by2 = bx*bx + by*by;
    Real cx2cy2 = cx*cx + cy*cy;

    Real ux = (
        ax2ay2 * (by - cy) +
        bx2by2 * (cy - ay) +
        cx2cy2 * (ay - by)
    ) / d;

    Real uy = (
        ax2ay2 * (cx - bx) +
        bx2by2 * (ax - cx) +
        cx2cy2 * (bx - ax)
    ) / d;

    Real dx = ux - ax;
    Real dy = uy - ay;

    out.center = {ux,uy};
    out.radius = std::sqrt(dx*dx + dy*dy);

    return out;
  }
}