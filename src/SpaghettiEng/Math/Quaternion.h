#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

// {} []

namespace Spg
{
   struct Quat
    {
      union
      {
        glm::vec4 d = {0,0,0,1}; //x,y,z,w
        struct{float x,y,z,w;}; 
      };
     
      // float x() {return d[0];}
      // float y() {return d[1];}  
      // float z() {return d[2];}  
      // float w() {return d[3];} 
    };

    // Quat operator * (const Quat& q1, const Quat& q2)
    // {
    //   float x = q1.d.x;
    // }

    // glm::mat4 QuatToMat4(const glm::quat& q)
    // {

    // }
}

