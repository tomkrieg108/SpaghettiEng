#pragma once

#include <unordered_map>

#include "SpaghettiEng/Render/Mesh/Mesh.h"                 //For MeshID

 // {} []
namespace Spg
{
  class ResourceManager;
  class Scene;
  class Camera;
  struct Material;
  

  class GLRenderer2
  {
  private:

    struct VertexArray
    {
      uint32_t vao;  //vertex array object
      uint32_t vbo; // bound vertex buffer object
      uint32_t ibo; // bound index buffer object
    };

  public:

    GLRenderer2() = delete;
    GLRenderer2(ResourceManager& resource_mgr);

    void InitGpuData(const Scene& scene);
    void Draw(const Scene& scene, const Camera& camera);

  private:
    void InitGpuData(MeshID mesh_id);
    void Draw(MeshID mesh_id, const Material& material,const Camera& camera);

  private:  
    std::unordered_map<uint32_t,VertexArray> m_vao_map; 

    ResourceManager& m_resource_mgr;
  };
} 