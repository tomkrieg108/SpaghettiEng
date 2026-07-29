# pragma once

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
  
}