#include "SpaghettiEng/Render/Mesh/MeshCache.h"

#include <vector>
#include <unordered_map>
#include <string>
//#include <cstdint>  //uint32_t
//#include <numbers>  //std::pi

#include "CoreLib/Core.h"
#include "SpaghettiEng/Render/Mesh/Mesh.h"
#include "SpaghettiEng/Render/Mesh/MeshData.h"

namespace Spg
{
  MeshId MeshCache::s_next_id = 0;
  std::unordered_map<MeshId, Mesh> MeshCache::s_mesh_map{};
  std::unordered_map<std::string, MeshId> MeshCache::s_name_to_id_map{};

  void MeshCache::Init()
  {
    SPG_INFO("Generating meshes...")
    GenerateGridMesh();
    GenerateCoordsMesh();
    SPG_INFO("Finished Generating meshes!")
  }

  MeshId MeshCache::GetMeshId(const std::string& mesh_name)
  {
    auto it = s_name_to_id_map.find(mesh_name);
    SPG_ASSERT( it != s_name_to_id_map.end());
    return it->second;
  }

  Mesh& MeshCache::GetMesh(MeshId mesh_id)
  {
    auto it = s_mesh_map.find(mesh_id);
    SPG_ASSERT( it != s_mesh_map.end());
    return it->second;
  }

  Mesh& MeshCache::GetMesh(const std::string& mesh_name)
  {
    auto id = GetMeshId(mesh_name);
    return GetMesh(id);
  }

  MeshId MeshCache::NextId()
  {
    auto next = MeshCache::s_next_id;
    ++MeshCache::s_next_id;
    return next;
  }

  MeshComponent MeshCache::GetMeshComponent(const std::string& mesh_name)
  {
    MeshComponent mc;
    mc.id = MeshCache::GetMeshId(mesh_name);
    return mc;
  }

  //void MeshCache::Add(Mesh& mesh) {}

  void MeshCache::GenerateGridMesh()
  {
    MeshLayout layout;
    layout.PushAttribute(MeshAttribute::Position);
    layout.PushAttribute(MeshAttribute::Color);

    Mesh mesh;
    mesh.id = MeshCache::NextId();
    mesh.name = "grid";
    mesh.type = MeshType::Grid;
    mesh.layout = layout;
    mesh.vertices = GenerateGridMeshData();
    
    s_mesh_map[mesh.id] = mesh;
    s_name_to_id_map[mesh.name] = mesh.id;
  }

  void MeshCache::GenerateCoordsMesh()
  {
    MeshLayout layout;
    layout.PushAttribute(MeshAttribute::Position);
    layout.PushAttribute(MeshAttribute::Color);

    Mesh mesh;
    mesh.id = MeshCache::NextId();
    mesh.name = "coords";
    mesh.type = MeshType::Coords;
    mesh.layout = layout;

    s_mesh_map[mesh.id] = mesh;
    s_name_to_id_map[mesh.name] = mesh.id;
  }

}