#pragma once
#include "GeomBase.h"


namespace Geom
{
  struct LineSeg2D
  {
    LineSeg2D() : start{0}, end{0}, dir_vec{0,0} {}

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

  //-------------------------------------

  template <class coord_type, uint32_t dim = 3>
  struct Line
  { //Vid 11
    static_assert(std::is_floating_point_v<coord_type>, "Type must be float or double");
    static_assert((dim == 2) || (dim == 3), "Line dimension must be 2 or 3");
    using glm_vec = glm::vec<dim, coord_type>;
    using glm_point = glm::vec<dim, coord_type>;

    Line(const glm_point& start_point, const glm_point& end_point) :
      start_point{ start_point }, end_point{ end_point }
    {
      direction = glm::normalize(end_point - start_point);
    }

    static Line LineFromPoints(const glm_point& start_point, const glm_point& end_point)
    {
      Line line{};
      line.start_point = start_point;
      line.end_point = end_point;
      line.direction = glm::normalize(end_point - start_point);
      return line;
    }
    static Line FromPointAndDirection(const glm_point& start_point, const glm_vec& direction)
    {
      Line line{};
      line.start_point = start_point;
      line.end_point = start_point + direction;
      line.direction = glm::normalize(direction);
      return line;
    }

    glm_point start_point;
    glm_point end_point;
    glm_vec direction; //normalized

  private:
    //todo - needs variable number of args
    Line(){};
  };

  using Line2d = Line<float, 2>;
  using Line3d = Line<float, 3>;


}
