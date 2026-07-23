#pragma once

#include <unordered_map>
#include <string>

#include "SpaghettiEng/Render/Mesh/Mesh.h"
/*
  {} []
*/
namespace Spg
{
  // Initialised on startup 

  //Todo - Similar thing for other resource types - make a templated class, 
  //todo - Can also look into entt::resource_cache, entt::resource
  class MeshCache
  {
    public:

      static void Init();

      static Mesh& GetMesh(MeshId mesh_id);
      static Mesh& GetMesh(const std::string& mesh_name);

      static MeshId GetMeshId(const std::string& mesh_name);
      static MeshId NextId();

      static MeshComponent GetMeshComponent(const std::string& mesh_name);

    private:
      static void GenerateGridMesh();
      static void GenerateCoordsMesh();

    private:  
      static MeshId s_next_id; 
      static std::unordered_map<MeshId ,Mesh> s_mesh_map;
      static std::unordered_map<std::string, MeshId> s_name_to_id_map;
  };
}