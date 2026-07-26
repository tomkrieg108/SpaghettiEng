# pragma once


#include <vector>
#include "SpaghettiEng/Render/Mesh/Mesh.h"

/*
  {}
  []
*/

namespace Spg
{
  struct Mesh;
  template<typename T> class ResourceCache;

  namespace MeshData
  {
    void Generate(ResourceCache<Mesh>& mesh_cache);
  }
  

  Mesh GenerateGridMesh();
  Mesh GenerateCoordsMesh();

  // Move the declarations into the cpp file later (and make static)
   std::vector<float> GenerateCoordsMeshData(float size = 1.0f);
   std::vector<float> GenerateGridMeshData(float size = 20.0f);

   std::vector<float> GeneratePlaneMeshData(float size);
   std::vector<float> GeneratePlaneMeshDataTM(float size);

   std::vector<float> GenerateCubeMeshData(float size);
   std::vector<float> GenerateCubeMeshDataTM(float size);

   std::vector<float> GenerateSphereMeshData();
   std::vector<float> GenerateSphereMeshDataTM();

}