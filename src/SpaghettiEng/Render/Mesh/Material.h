#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"

//  {}  []

namespace Spg
{
  struct Material
  {
    ShaderID shader_id;
    glm::vec3 colour = glm::vec3(0.0f,0.2f,0.8f);
    // Textures
    // Roughness
    // Metalic
    // Colour 
  };

}

