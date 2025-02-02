#pragma once
#include "GeomBase.h"


namespace Geom
{
  struct LineSeg2D
  {
    LineSeg2D(const Point2d& p_start, const Point2d& p_end) : start{p_start}, end{p_end}
    {
      dir_vec = end - start;
      dir_vec = glm::normalize(dir_vec);
    }
    void Set(const Point2d& p_start, const Point2d& p_end)
    {
      start=p_start;
      end=p_end;
      dir_vec = end - start;
      dir_vec = glm::normalize(dir_vec);   
    }

    float Length() 
    {
      return glm::length(dir_vec);
    }

    // [-pi,pi] in radians
    float Angle() 
    {
      const float theta = std::atan2f(dir_vec.y, dir_vec.x);
      return glm::degrees(theta);
    }
    
    Point2d start, end;
    glm::vec2 dir_vec;
    
  };
}
