#pragma once

#include <unordered_map>

#include "SpaghettiEng/Render/Mesh/Mesh.h"   //For MeshID
#include "SpaghettiEng/Scene/Transform.h"

 // {} []
namespace Spg
{
  class ResourceManager;
  class Scene;
  class Camera;
  struct Material;
  //struct Transform; //Error because wrapped in namespace - need to #include
  
  class GLRenderer2
  {
  public:

    GLRenderer2() = delete;
    GLRenderer2(ResourceManager& resource_mgr);

    void InitGpuData(MeshID mesh_id);

    void Draw(MeshID mesh_id, const Material& material, 
      const Camera& camera, const Transform& camera_transform);

  private:

    struct VertexArray
    {
      uint32_t vao;  //vertex array object
      uint32_t vbo; // bound vertex buffer object
      uint32_t ibo; // bound index buffer object
    };

  private:   
    //Utility functions
    static uint32_t GLAttributeBaseType(const MeshAttribute& mesh_attribute);
    static uint32_t GLTopology(const Mesh& mesh);
    static uint32_t GLUsage(const Mesh& mesh);  

  private:  
    std::unordered_map<uint32_t,VertexArray> m_vao_map; 
    ResourceManager& m_resource_mgr;
  };
} 