# pragma once

#include "SpaghettiEng/Render/Mesh/Mesh.h"

#include <vector>
#include <unordered_map>
#include <string>

/*
  {}
  []
*/

namespace Spg
{

  std::vector<float> GenerateCoordsMeshData(float size = 1.0f);
  std::vector<float> GenerateGridMeshData(float size = 20.0f);

  std::vector<float> GeneratePlaneMeshData(float size);
  std::vector<float> GeneratePlaneMeshDataTM(float size);

  std::vector<float> GenerateCubeMeshData(float size);
  std::vector<float> GenerateCubeMeshDataTM(float size);

  std::vector<float> GenerateSphereMeshData();
  std::vector<float> GenerateSphereMeshDataTM();

}