#pragma once

#include "GeomBase.h"

namespace Geom {

	struct AABB 
  {
		float x_min;
		float x_max;
		float y_min;
		float y_max;

		bool IsInside(Point2d& point) {
			if (x_min <= point.x && point.x <= x_max
				&& y_min <= point.y && point.y <= y_max)
				return true;
			return false;
		}
	};
}