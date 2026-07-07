# pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

/*
  {}
  []
*/

namespace Spg
{

  enum class MeshPrimitive {Plane, Cube, Sphere, Other};

  //default to AOS (interleaved attributes)
  //SOA only for specific scenarios - do later...maybe!

  struct MeshHandle
  {
    uint32_t handle;
  };

  // Check GLBufferLayout.h 
  struct MeshFormat
  {

  };

  struct MeshVertex
  {

  };

  struct Mesh
  {
    MeshFormat format;
    std::vector<float> data;
  };

  // Initialised on startup by AssetManager
  class MeshRepo
  {
    public:


    private:
      std::unordered_map<MeshHandle,Mesh> m_mesh_map;
  };

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