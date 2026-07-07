# pragma once

#include <vector>

/*
  {}
  []
*/

namespace Spg
{
  namespace MeshData
  {
    std::vector<float> GenerateCoordsMeshData(float size);
    std::vector<float> GenerateGridMeshData(float size);

    std::vector<float> GeneratePlaneMeshData(float size);
    std::vector<float> GeneratePlaneMeshDataTM(float size);

    std::vector<float> GenerateCubeMeshData(float size);
    std::vector<float> GenerateCubeMeshDataTM(float size);

    std::vector<float> GenerateSphereMeshData();
    std::vector<float> GenerateSphereMeshDataTM();

  }

}