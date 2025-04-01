#pragma once
#include <SpaghettiEng/SpaghettiEng.h>
#include <SpaghettiEng/Camera/Camera2D.h>
#include <SpaghettiEng/OpenGL32/GL32Renderer.h>
#include <Geometry/Geometry.h>

using namespace std::string_literals;

namespace Spg
{
  class BubbleLayer : public Layer
  {
  public:

    struct Drawable
    {
      Drawable() = default;

      Drawable(GLShader* shader_, GLRenderer::DrawMode draw_mode_, bool enabled_ = false) :
        shader{shader_}, draw_mode{draw_mode_}, enabled{enabled_} {}
             
      Drawable(const GLVertexBuffer& vbo, GLShader* shader_, GLRenderer::DrawMode draw_mode_, bool enabled_ = false) :
        shader{shader_}, draw_mode{draw_mode_}, enabled{enabled_} {
          VAO.AddVertexBuffer(vbo);
        }
        
      void AddVBO(const GLVertexBuffer& vbo) {
        VAO.AddVertexBuffer(vbo);
      }  
      GLVertexArray VAO;
      GLShader* shader = nullptr;
      GLRenderer::DrawMode draw_mode;
      bool enabled = false;
    }; 

    struct PolygonRenderData
    {
      struct Layout
      {
        glm::vec3 pos;
        glm::vec4 col;
      };

      std::size_t NumPolygons() {
        return point_meshes.size();
      }

      std::vector<Geom::Point2d>& GetPolygonPoints(const std::string& name) {
        auto itr = polygon_points.find(name);
        SPG_ASSERT(itr != polygon_points.end());
        return itr->second;
      }

      void AddPolygon(const std::string& name, const std::vector<Geom::Point2d>& points, GLShader* shader);
      void InitialiseMonotoneAlgoVisuals(const std::string& name,GLShader* shader);
      void UpdateMonotoneAlgoSweepLineDisplay();
      void UpdateMonotoneAlgoActiveEdgeDisplay();

      void SetPointColor(const std::string& name, uint32_t index, const glm::vec4& color);
      void SetPointColor(const std::string& name, const glm::vec4& color);
      void SetPointColor(const std::string& name, const std::vector<uint32_t>& indicies, const glm::vec4& color);

      void SetSegColor(const std::string& name, uint32_t index, const glm::vec4& color);
      void SetSegColor(const std::string& name, const glm::vec4& color);
      void SetSegColor(const std::string& name, const std::vector<uint32_t>& indicies, const glm::vec4& color);
     
      std::unordered_map<std::string, Drawable> point_drawables;
      std::unordered_map<std::string, Drawable> seg_drawables;

      std::unordered_map<std::string, std::vector<Layout>> point_meshes;
      std::unordered_map<std::string, std::vector<Layout>> seg_meshes;

      std::unordered_map<std::string, std::vector<Geom::Point2d>> polygon_points;

      Geom::Monotone_V2::MonotoneSpawner5000 monotone_spawner;

      static BufferLayout s_buffer_layout; 
    };

    

  public:
    BubbleLayer(AppContext& app_context, const std::string& name);
    ~BubbleLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(double time_step) override;
    void OnEvent(Event& event) override;
    void OnImGuiRender() override;  
    void SetCanvasSize(float canvas_size);

  private:
    void OnWindowResize(EventWindowResize& e);
    void OnMouseMoved(EventMouseMoved& e);
    void OnMouseScrolled(EventMouseScrolled& e);
    void OnMouseButtonPressed(EventMouseButtonPressed& e);
    void OnMouseButtonReleased(EventMouseButtonReleased& e);

    void Create2DGrid();  
   
    void CreateTriangulatedPolygon(uint32_t vertex_count);
    void CreateMonotonePartitionedPolygon(uint32_t vertex_count);
    void CreateCircle(uint32_t vertex_count);
    void CreatePoints(uint32_t point_count);
    void CreateConvexHull(uint32_t point_count);
     void GeomTest();

    void UpdateCanvas();

  private:
    Window& m_window;
    GLRenderer& m_renderer;
    Camera2D& m_camera;
    CameraController2D& m_camera_controller;
    Utils::SpdLogger m_logger;
    Scope<GLShader> m_shader = nullptr;
    bool m_pan_enabled = false;
    float m_canvas_size = 500.0f;

    PolygonRenderData m_polygon_render_data;
    std::unordered_map<std::string, Drawable> m_drawables;
    
    

    GLVertexArray m_vao_grid;
    GLVertexArray m_vao_polygon;
    GLVertexArray m_vao_polygon_diagonals;
    GLVertexArray m_vao_circle;
    GLVertexArray m_vao_points;
    GLVertexArray m_vao_convex_hull;
  };

  class BubbleSoup : public Application
  {
  public:
    BubbleSoup(const std::string& title);
    ~BubbleSoup();
  private:
  };

  void AppPrintHello();
}
