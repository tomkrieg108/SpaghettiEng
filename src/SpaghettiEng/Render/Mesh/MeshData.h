# pragma once

#include "SpaghettiEng/Render/Mesh/MeshFormat.h"


#include <vector>
#include <unordered_map>

/*
  {}
  []
*/

namespace Spg
{


  enum class MeshPrimitive {Plane, Cube, Sphere, Other};

  
  struct Mesh
  {
    MeshFormat format;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    uint32_t handle;

  private:
    static uint32_t s_next_handle;
  };

  // Initialised on startup by AssetManager
  class MeshRepo
  {
    public:


    private:
      std::unordered_map<uint32_t,Mesh> m_mesh_map;
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


    Mesh GeneratePlaneMesh(float plane_size, MeshFormat mesh_format);
    Mesh GenerateCubeMesh(float cube_size, MeshFormat mesh_format);
  }

}