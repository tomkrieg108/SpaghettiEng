#pragma once
#include <vector>

namespace Geom
{
  // BufferLayout layout =
  // {
  //   {"a_position", ShaderDataType::Float3},
  //   {"a_color", ShaderDataType::Float4}
  // };
  std::vector<float> CreateGrid_XY(float grid_size, float unit_size);
  

  
}
