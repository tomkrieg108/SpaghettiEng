#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer.h"

#include <unordered_map>
#include <cstdint> 

#include <glm/mat4x4.hpp>
#include <glad/gl.h> 

#include "CoreLib/Core.h"

#include "SpaghettiEng/Core/Window.h"

#include "SpaghettiEng/Resource/ResourceCache.h"
#include "SpaghettiEng/Resource/ResourceManager.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"
#include "SpaghettiEng/Render/Camera/Camera.h"
#include "SpaghettiEng/Render/Mesh/Mesh.h"
#include "SpaghettiEng/Render/Mesh/Material.h"

#include "SpaghettiEng/Scene/Entity.h"
#include "SpaghettiEng/Scene/Registry.h"
#include "SpaghettiEng/Scene/Scene.h"
#include "SpaghettiEng/Scene/SceneManager.h"
//#include "SpaghettiEng/Scene/Transform.h"
#include "SpaghettiEng/Math/Transform.h"

// {} []
namespace Spg
{

  GLRenderer::GLRenderer(Window& window, SceneManager& scene_mgr, 
    ResourceManager& resource_mgr) :
    m_window{window},
    m_resource_mgr{resource_mgr},
    m_scene_mgr{scene_mgr}
  {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(8.0f); //can be at least 64.0
    // glEnable(GL_BLEND); 
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Init();
  }

  void GLRenderer::Init()
  {
    // Create Uniform Buffers (UBO's) fo
    // 1) per-frame data 
    // 2) Per render-pass data
    glGenBuffers(1, &m_ubo_frame_data);
    glGenBuffers(1, &m_ubo_pass_data);

    glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_frame_data);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(FrameData), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ubo_frame_data);

    glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_pass_data);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PassData), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_ubo_pass_data);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Initialise Scene Camera
    auto& scene = m_scene_mgr.GetActiveScene();
    auto& scene_camera = m_scene_mgr.GetSceneCamera();
    scene_camera.SetAspectRatio(m_window.GetAspectRatio());


    // Generate Vertex Array's (VAO) and associated VBO/EVO for all mesh objects
    auto& reg = scene.GetRegistry();
    auto mesh_view = reg.GetAllEntitiesWith<MeshHandle>();
     //* NOTE ent and mesh_view are raw EnTT data types - not encapsulated in registry
    for(auto ent : mesh_view)
    {
      auto& mesh_handle = reg.GetComponent<MeshHandle>(Entity{ent});
      InitGpuData(mesh_handle.mesh_id);
    }
  }

  void GLRenderer::InitScene(const Scene& scene)
  {
    
  }

  void GLRenderer::BeginFrame(const FrameData& frame_data)
  {
     glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_frame_data);
     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FrameData), (void*)(&frame_data));
     glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void GLRenderer::BeginPass(const PassData& pass_data)
  {
     glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_pass_data);
     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PassData), (void*)(&pass_data));
     glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void GLRenderer::EndFrame()
  {
  }

  void GLRenderer::EndPass()
  {
  }

  void GLRenderer::DrawScene(const Scene& scene)
  {
    auto& scene_camera = m_scene_mgr.GetSceneCamera();
    auto& camera_transform = m_scene_mgr.GetSceneCameraTransform();

    FrameData frame_data;
    frame_data.light_pos = glm::vec3(1.0f,3.0f,2.0f);
    frame_data.light_colour = glm::vec3(1.0f,1.0f,1.0f);
    BeginFrame(frame_data);

    PassData pass_data;
    pass_data.proj = scene_camera.GetProjMatrix();
    pass_data.view = camera_transform.Inverse();
    //pass_data.camera_position = camera_transform.Position();
    pass_data.camera_position = camera_transform.position;
    BeginPass(pass_data);

    auto& reg = scene.GetRegistry();
    auto view = reg.GetAllEntitiesWith<Transform, MeshHandle, Material>();

    //* NOTE ent and view are raw EnTT data types - not encapsulated in registry
    for(auto [ent, transform, mesh_handle, mat] : view.each())
    {
      RenderCommand cmd;
      cmd.mesh_id = mesh_handle.mesh_id;
      cmd.material = mat;
      //cmd.model_mat = transform.matrix;
      cmd.model_mat = transform.ToMat4();

      Draw(cmd);
    }

    EndPass();
    EndFrame();
  }

  void GLRenderer::DrawActiveScene()
  {
    const auto& active_scene = m_scene_mgr.GetActiveScene(); 
    DrawScene(active_scene);
  }

  void GLRenderer::Draw(const RenderCommand& cmd)
  {
     auto& shader = m_resource_mgr.GetResourceCache<GLShader>().Get(cmd.material.shader_id);
    const auto& mesh = m_resource_mgr.GetResourceCache<Mesh>().Get(cmd.mesh_id);

    shader.Bind();
    shader.SetUniformMat4f("u_model", cmd.model_mat);

    if(shader.GetName() == "Basic Lighting Shader") 
    {
      glm::vec3 col = cmd.material.colour;
      shader.SetUniform3f("u_object_colour",col);
    }
      
    auto it = m_vao_map.find(cmd.mesh_id.index);
    SPG_ASSERT(it != m_vao_map.end());
    auto vert_arr = it->second;
    glBindVertexArray(vert_arr.vao);

    if(mesh.index_count == 0)
      glDrawArrays(GLTopology(mesh), 0, mesh.vertex_count);
    else
    {
      //Turn on wireframe mode
      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawElements(GLTopology(mesh), mesh.index_count, GL_UNSIGNED_INT, 0);
      
      //Switch back to normal filled mode when done
      //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
      

    glBindVertexArray(0);
    shader.Unbind();
  }

  void GLRenderer::InitGpuData(MeshID mesh_id)
  {
    auto it = m_vao_map.find(mesh_id.index);
    if(it != m_vao_map.end())
      return; //this mesh already been processed

    const auto& mesh = m_resource_mgr.GetResourceCache<Mesh>().Get(mesh_id);
    
    uint32_t vao = 0, vbo = 0, ebo = 0;

    glGenVertexArrays(1, &vao); //V3.0+
    glBindVertexArray(vao);

    glGenBuffers(1,&vbo); //v2.0+
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_buffer.size(), 
      (void *)mesh.vertex_buffer.data(), GLUsage(mesh));

    if(mesh.index_count > 0) 
    {
      glGenBuffers(1,&ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(uint32_t), 
        (void *)mesh.indices.data(), GLUsage(mesh));
    }

    const auto& layout = mesh.layout;
    uint32_t attribute_idx = 0;
    for(auto& attr : layout.attribute_list) 
    {
      glVertexAttribPointer(attribute_idx, AttributeComponentCount(attr.attribute_type),
        GLAttributeBaseType(attr), GL_FALSE, layout.size_in_bytes, 
        (void*)(attr.offset_in_bytes));  

      glEnableVertexAttribArray(attribute_idx);
      ++attribute_idx;
    }

    glBindVertexArray(0);
    m_vao_map[mesh_id.index] = VertexArray{vao,vbo,ebo};
  }

  
  // ==== static Utility functions ====================================
  uint32_t GLRenderer::GLAttributeBaseType(const MeshAttribute& mesh_attribute)
  {
    switch(mesh_attribute.base_type)
    {
      case MeshAttributeBaseType::Float: return GL_FLOAT; 
      case MeshAttributeBaseType::Int: return GL_INT;
      case MeshAttributeBaseType::UInt: return GL_UNSIGNED_INT; 
      case MeshAttributeBaseType::Bool: return GL_BOOL; 
    }
    SPG_ERROR("Unknown GL base type: {} ", (uint32_t)mesh_attribute.base_type);
    return 0;
  } 

  uint32_t GLRenderer::GLTopology(const Mesh& mesh)
  {
    switch(mesh.topology)
    {
      case MeshTopology::Triangles: return GL_TRIANGLES;
      case MeshTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
      case MeshTopology::Lines: return GL_LINES;
      case MeshTopology::Points: return GL_POINTS;
    }
    SPG_ERROR("Unknown GL mesh topology: {} ", (uint32_t)mesh.topology);
    return 0;
  } 

  uint32_t GLRenderer::GLUsage(const Mesh& mesh)
  {
    switch(mesh.usage)
    {
      case MeshUsage::Static: return GL_STATIC_DRAW;
      case MeshUsage::Dynamic: return GL_DYNAMIC_DRAW;
    }
    SPG_ERROR("Unknown GL mesh usage: {} ", (uint32_t)mesh.usage);
    return 0;
  } 
  
} //namespace Spg


#if 0

  // 1. Agnostic enums to represent primitive types
  enum class MeshTopology 
  {
    Triangles,
    Lines,
    Points
  };

  // 2. A handle system to abstract GPU buffers (just unique IDs, not GLuint)
  using MeshHandle = uint32_t;
  using MaterialHandle = uint32_t;

  // 3. The raw command packet submitted by game logic
  struct RenderCommand 
  {
    MeshHandle meshId;
    MaterialHandle materialId;
    
    // Transform matrix (typically 16 floats for a 4x4 matrix)
    std::array<float, 16> transformMatrix;
    
    MeshTopology topology = MeshTopology::Triangles;
    uint32_t indexCount = 0;
    uint32_t indexOffset = 0;
    
    // Layer or sorting keys (crucial for optimizing draw orders later)
    uint32_t renderQueueLayer = 0; // e.g., 0 = Opaque, 1 = Transparent, 2 = UI
    float depthFromCamera = 0.0f;  
  };

  

  struct RenderCommand2 {
    // 64-bit Sorting Key (Material ID, Depth, Layer packed into bits)
    uint64_t sort_key; 

    MeshHandle mesh_id;
    MaterialHandle material_id; // Use an ID/Handle instead of full struct

    // Transform data (Only what changes per object)
    glm::mat4 model;

    // Optional: Draw arguments for indirect rendering
    uint32_t index_count;
    uint32_t first_index;
    uint32_t base_vertex;
    uint32_t base_instance;
};


  class Renderer {
  public:
      Renderer() = default;

      // Game loop calls this to queue up objects
      void Submit(const RenderCommand& cmd) {
          m_CommandQueue.push_back(cmd);
      }

      // Called once at the end of the frame
      void EndFrame() {
          // Step 1: Optimize the stream
          SortQueue();

          // Step 2: Execute commands on the current API
          ExecuteQueue();

          // Step 3: Clear the queue for the next frame
          m_CommandQueue.clear();
      }

  private:
      std::vector<RenderCommand> m_CommandQueue;

      void SortQueue() {
          // Sort commands to prevent unnecessary GPU state changes
          std::sort(m_CommandQueue.begin(), m_CommandQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
              // Priority 1: Sort by layer (Opaque before Transparent)
              if (a.renderQueueLayer != b.renderQueueLayer) {
                  return a.renderQueueLayer < b.renderQueueLayer;
              }
              // Priority 2: Sort by material to minimize shader switching cost
              if (a.materialId != b.materialId) {
                  return a.materialId < b.materialId;
              }
              // Priority 3: Sort by mesh to minimize VAO switching cost
              return a.meshId < b.meshId;
          });
      }

      void ExecuteQueue() {
          // This is the ONLY place where OpenGL code lives.
          // When you port to Vulkan, you only rewrite this single function.
          for (const auto& cmd : m_CommandQueue) {
              
              // 1. Map MaterialHandle to actual GL Shader / Textures
              // (Your engine will need an internal registry lookup)
              GLuint glShader = LookupGLShader(cmd.materialId);
              glUseProgram(glShader);
              
              // Upload the transform matrix to a shader uniform
              GLint transformLoc = glGetUniformLocation(glShader, "u_Transform");
              glUniformMatrix4fv(transformLoc, 1, GL_FALSE, cmd.transformMatrix.data());

              // 2. Map MeshHandle to actual GL VAO
              GLuint glVao = LookupGLVao(cmd.meshId);
              glBindVertexArray(glVao);

              // 3. Translate topology enum to OpenGL enum
              GLenum glTopology = (cmd.topology == MeshTopology::Lines) ? GL_LINES : GL_TRIANGLES;

              // 4. Issue the draw call
              glDrawElements(
                  glTopology, 
                  cmd.indexCount, 
                  GL_UNSIGNED_INT, 
                  (void*)(uintptr_t)(cmd.indexOffset * sizeof(uint32_t))
              );
          }
      }

      // Dummy lookup helpers for illustration
      GLuint LookupGLShader(MaterialHandle id) { return 1; }
      GLuint LookupGLVao(MeshHandle id) { return 1; }
  };


  void main_prog()
  {
    Renderer renderer;
    
    // Main Game Loop
    while (gameIsRunning) {
        // 1. Process Input & Update Game Objects
        // ... update positions ...

        // 2. Render Submission Phase
        for (auto& entity : gameEntities) {
            RenderCommand cmd;
            cmd.meshId = entity.meshAssetId;
            cmd.materialId = entity.materialAssetId;
            cmd.transformMatrix = entity.GetTransformMatrixFloats();
            cmd.indexCount = entity.meshIndexCount;
            cmd.renderQueueLayer = entity.isTransparent ? 1 : 0;

            renderer.Submit(cmd); // Fast, just copies data into vector
        }

        // 3. Execution Phase
        renderer.EndFrame(); // Sorts, binds OpenGL state, and pushes to GPU
    }
#endif

    /*
    
    Why This Is Perfect for a Future Vulkan Port
    
    Command Generation Boundary: Inside ExecuteQueue(), instead of running sequential OpenGL commands on the main thread, a Vulkan backend can instantly pass chunks of m_CommandQueue across 4 or 8 background worker threads.
    
    Pre-Baked Command Buffers: Each worker thread records its dedicated slice of commands into a VkCommandBuffer.
    
    Single Submission: The threads return their recorded buffers to the main renderer thread, which submits them all to the GPU queue in a single, ultra-low-overhead API call. Your gameplay logic loop won't change at all!
    
    */

