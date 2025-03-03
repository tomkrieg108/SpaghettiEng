#pragma once
#include "GeomBase.h"


namespace Geom
{
  template <typename coord_type>
  struct PlaneT
  { //vid 11
    static_assert(std::is_floating_point_v<coord_type>, "Type must be float or double");
    using glm_vec3 = glm::vec<3, coord_type>;
    using glm_point3 = glm::vec<3, coord_type>;
    

    glm_vec3 normal;
    float d;		//ax + dy + cz = d  
    //d is dot product between normal vector and a known point on the plane
    
    PlaneT(const glm_vec3& normal, float d) 
      : normal{ normal}, d{d}
    { 
      this->normal = glm::normalize(normal);
    }

    //Todo - might want this one too
    PlaneT(const glm_vec3& normal,  const glm_point3& point);

    //Note: if plane is viewed from direction of normal, points will be CCW
    PlaneT(const glm_point3& p1, const glm_point3& p2, const glm_point3& p3)
    {
      auto p1_p2 = p2 - p1;
      auto p1_p3 = p3 - p1;

      normal = glm::cross(p1_p2, p1_p3);
      normal = glm::normalize(normal); //todo - does normlising change d?
      d = glm::dot(normal, p1);
    }
  };

  using Plane = PlaneT<float>;
}