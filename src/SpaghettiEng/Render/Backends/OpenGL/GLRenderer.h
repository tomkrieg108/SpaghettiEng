#pragma once

#include <unordered_map>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "SpaghettiEng/Render/Mesh/Mesh.h"   //For MeshID
#include "SpaghettiEng/Render/Mesh/Material.h"
//#include "SpaghettiEng/Scene/Transform.h"
#include "SpaghettiEng/Math/Transform.h"

 // {} []
namespace Spg
{
  class Window;
  class ResourceManager;
  class SceneManager;
  class Scene;
  class Camera;
  struct Material;
  //struct Transform; //Error because wrapped in namespace - need to #include
  
  class GLRenderer
  {
  public:

    //ensures sizeof(FrameData) is 32, not 28
    //Include alignas(16) for struct too
    struct alignas(16) FrameData 
    {
      alignas(16) glm::vec3 light_pos;
      alignas(16) glm::vec3 light_colour;
      // float total_time;
    };

    static_assert(sizeof(FrameData) == 32, "FrameData size must be 32 bytes for std140");
    static_assert(offsetof(FrameData, light_colour) == 16, "light_colour must be at offset 16");

    struct PassData
    {
      glm::mat4 view;
      glm::mat4 proj;
      glm::vec3 camera_position;
    };

    struct RenderCommand
    {
      MeshID mesh_id;
      Material material; //* better to make this than ID than a struct
      glm::mat4 model_mat;
    };

  public:

    GLRenderer() = delete;
    GLRenderer(Window& window, SceneManager& scene_mgr, 
      ResourceManager& resource_mgr);

    void Init();
    void InitScene(const Scene& scene);

    void BeginFrame(const FrameData& frame_data);
    void BeginPass(const PassData& pass_data);
    void EndFrame();
    void EndPass();
    
    void DrawScene(const Scene& scene);
    void DrawActiveScene();

    void InitGpuData(MeshID mesh_id); 
    void Draw(const RenderCommand& cmd);
    
  private:

    struct VertexArray
    {
      uint32_t vao;  //vertex array object
      uint32_t vbo; // bound vertex buffer object
      uint32_t ebo; // bound index buffer object
    };
  
  private:   
    //Utility functions
    static uint32_t GLAttributeBaseType(const MeshAttribute& mesh_attribute);
    static uint32_t GLTopology(const Mesh& mesh);
    static uint32_t GLUsage(const Mesh& mesh);  

  private:
    Window& m_window;
    SceneManager& m_scene_mgr;
    ResourceManager& m_resource_mgr;
    
    std::unordered_map<uint32_t,VertexArray> m_vao_map; 

    uint32_t m_ubo_frame_data;
    uint32_t m_ubo_pass_data;
  };
} 

//RenderCammand 
/*
  Why Copying Data is Actually Better:
  If you pass references/pointers to game objects instead of copying data, your rendering system faces two severe bottlenecks:Cache Misses (The "Pointer Chasing" Problem): Game objects are usually scattered all over the CPU heap. If your renderer uses pointers/references to look up matrices and materials during the draw loop, the CPU will constantly stall waiting for memory to load. Copying data into a contiguous array (std::vector<Command>) creates perfect cache locality. The CPU can blast through the draw calls sequentially.Multithreading & Frame Pipetlining: Modern renderers (especially in Vulkan) submit draw calls from a dedicated render thread while the main thread works on the next frame's game logic. If you pass references, the main thread might modify an object's matrix while the render thread is trying to read it, causing race conditions. Copying the data creates a frozen "snapshot" of the frame.

  How to Optimize the Command Struct:
  While the pattern is correct, your specific struct layout can be optimized to reduce memory footprint and improve performance.1. Strip the View and Projection MatricesThe view and proj matrices are uniform across the entire frame (or per-pass/per-camera). Storing them in every single command is redundant and bloats the struct.Recommendation: Move view and proj to a global frame context or pass them once to Renderer::BeginFrame(). Keep only the model matrix in the command.2. Keep the Struct Small (Crucial for Sorting)To avoid rendering bottlenecks, you must sort your render queue (e.g., by Material/Pipeline to minimize state changes, or front-to-back for opaque objects to reduce overdraw). Sorting an array of large structs is slow.Recommendation: Pack your command tightly, or use an Index Buffer approach where you sort an array of small 64-bit keys (storing sort depth and a 32-bit index into your actual command array).
  
  The Ultimate Optimization: 
  Indirect Rendering (Vulkan/Modern GL)If you are targetting Vulkan or modern OpenGL (4.3+), you can look into GPU-Driven Rendering via vkCmdDrawIndexedIndirect or glMultiDrawElementsIndirect.With this approach, your Renderer::Submit function doesn't even hold matrices in CPU memory. It writes the object transforms directly into a persistent mapped GPU Storage Buffer (SSBO), and your RenderCommand matches the exact layout the GPU expects for an indirect draw call. The CPU does zero matrix copying to local structs—it writes straight to GPU visible memory.
  
  
  */

//Render pipeline
/*

1. Shader ID: 
Material vs. Separated?In professional renderers, the Shader ID is almost always part of the Material.A material is fundamentally defined by two things: the pipeline/shader it uses, and the parameters/textures it feeds into that shader. Having a material without a shader ID makes it an abstract collection of numbers and textures with no instructions on how to draw them.However, the shader itself does not care where its data comes from. To solve the dilemma of frame-wide data versus model-specific data, renderers use a highly structured hierarchy based on how often data changes

2. The Standard Architecture: Data Frequency SplittingYou absolutely want separate data structures for the Frame, Pass, Material, and Model.Updating data on the GPU is expensive. If you bunch frame data (camera) together with model data (transform), you force the GPU to constantly re-bind and re-upload variables that haven't changed. Modern APIs like Vulkan and DirectX 12 are explicitly designed around this frequency of change via Descriptor Set layouts / Root Signature spaces.Here is how the data structures are normally split and bound in a frame:

[Space 0] Per-Frame Data    (Bind ONCE per frame)
   └── Global Time, Lighting Data, Shadow Maps
   
[Space 1] Per-Pass Data     (Bind ONCE per render pass/camera view)
   └── View Matrix, Projection Matrix, Camera Position
   
[Space 2] Per-Material Data (Bind ONLY when switching materials)
   └── Albedo Texture Handle, Roughness/Metalness scalars, Shader ID
   
[Space 3] Per-Model Data    (Bind PER DRAW CALL or stream via dynamic offsets)
   └── Model/World Matrix, Animation Bones, Mesh ID

3. What the Code Looks LikeHere is how this hierarchy looks structurally on the CPU, mapped cleanly to your pipeline:

// 1. Updated once at the start of the frame
struct FrameUniforms {
    float total_time;
    glm::vec4 ambient_light_color;
    DirectionalLight main_light;
};

// 2. Updated per camera view (e.g. Main Camera, Shadow Pass Camera, Reflection Probe)
struct PassUniforms {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 camera_position;
};

// 3. Stored in an asset library; referenced by commands
struct Material {
    ShaderID shader_id;       // Maps to a specific Vulkan/GL Pipeline State (PSO)
    TextureHandle albedo_tex;
    float roughness;
    float metalness;
};

// 4. Generated and submitted every frame to the Render Queue
struct RenderCommand {
    MeshID mesh_id;
    MaterialID material_id;   // The command points to the material
    glm::mat4 model;          // Unique per-instance data
};

How this executes on the GPU (The Render Loop)When your renderer processes the accumulated RenderCommand vector, it sorts them by MaterialID first. The loop avoids redundant binding calls like a pro:Bind Frame Data: Upload and bind FrameUniforms (Space 0).Loop through Render Passes (e.g., Forward Opaque Pass):Bind PassUniforms for the current camera (Space 1).Loop through sorted Render Commands:Is this command using a new material?Yes \(\rightarrow \) Bind the material's shader_id (Pipeline), bind its textures/scalars (Space 2).No \(\rightarrow \) Skip binding! (Massive CPU/GPU driver savings).Bind Model Data: Push the model matrix (Space 3) via a Push Constant or a dynamic Uniform Buffer offset.Draw: Issue the GL/Vulkan draw call for the mesh_id.Would you like to look at how Push Constants (OpenGL/Vulkan) or Dynamic Uniform Buffers are used to quickly stream that per-model model matrix without creating thousands of tiny descriptor allocations?

*/

//Scene verses renderer responsibilities
/*
You should definitely go with renderer.draw(scene) (or more accurately, renderer.submit(scene)).In modern graphics programming, scene.draw() is considered an anti-pattern. While scene.draw() is common in older tutorials or simple object-oriented toy engines, it quickly breaks down when building a high-performance renderer for OpenGL or Vulkan.Here is why separating them is the industry standard, along with how their responsibilities should be cleanly split.

The Fundamental Flaw of scene.draw()If a Scene draws itself, it means your spatial data structures (nodes, transforms, bounding boxes) must intimately understand graphics API concepts (command buffers, descriptor sets, pipelines, and render passes).This creates tight coupling and violates the Single Responsibility Principle. If you decide to switch your backend from OpenGL to Vulkan, or if you want to add a simple shadow-mapping pass, you would have to completely rewrite your Scene and GameObject classes.

The Modern Split of ResponsibilitiesInstead of thinking about "drawing", think of the pipeline in two distinct phases: Culling & Gathering (Scene responsibility) and Execution & Command Submission (Renderer responsibility).

1. The Scene Class (The "What")The scene is a data provider. It manages the game world, update logic, and spatial relationships. It has no idea how things are rendered; it only knows where things are in the world.

Responsibilities:Holds the scene graph or Entity Component System (ECS).Updates hierarchical matrices (parent/child transforms).Performs frustum culling (using an octree or bounding boxes) to filter out objects the camera cannot see.Exposes a way for the renderer to query visible objects.

The Renderer Class (The "How")The renderer is a data consumer. It takes a raw list of visible objects and figures out the most optimal, API-specific way to push them to the GPU.

Responsibilities:Manages OpenGL/Vulkan resources (UBOs, textures, pipelines, framebuffers).Sorts render commands by Material ID to minimize state changes, or front-to-back for depth optimizations.Translates game-world objects into low-level API commands.

What the Code Looks Like in PracticeIn a production engine, the workflow usually passes through a clean middle ground. The scene produces an abstraction, and the renderer consumes it.

void GameLoop() {
    // 1. Update gameplay, physics, and animations
    scene.Update(deltaTime);

    // 2. Clear previous frame's graphics commands
    renderer.BeginFrame();

    // 3. Extract WHAT needs to be drawn. 
    // The scene returns a lightweight list of visible entities.
    auto visibleObjects = scene.GetVisibleObjects(mainCamera);

    // 4. Submit the data to the renderer
    for (const auto& entity : visibleObjects) {
        // Construct that lightweight RenderCommand we discussed earlier!
        RenderCommand cmd;
        cmd.mesh_id = entity.mesh;
        cmd.material_id = entity.material;
        cmd.model = entity.transform;

        renderer.Submit(cmd);
    }

    // 5. Execute: Sort commands, bind UBOs, and issue draw calls
    renderer.EndFrame(); 
}

Why this structure winsMulti-Camera / Multi-Pass support is trivial: If you want to render a reflection probe or a shadow map, you just call scene.GetVisibleObjects(shadowCamera) and submit those to a shadow pass inside the renderer. The Scene class doesn't need to change at all.API Agnostic: Your Scene class can remain 100% pure C++. You can swap out an OpenGLRenderer for a VulkanRenderer seamlessly because the Scene only outputs raw transforms, mesh IDs, and material IDs.

*/