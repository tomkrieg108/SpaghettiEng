#pragma once
#include <SpaghettiEng/SpaghettiEng.h>
#include <Geometry/Geometry.h>

namespace Spg
{
  
  class DefaultLayer : public Layer
  {

  private:

    enum class MeshType
    {
      Polygon, Circle, LineSet, PointSet
    };

    struct Label
    {
      std::string text;
      Geom::Point2d pos;
    };
    
    struct Mesh
    {
      Mesh()=default;
      Mesh(const Mesh&) = default;
      Mesh(Mesh&&) = default;

     // Mesh& operator=(const Mesh&) = default;
      Mesh& operator=(const Mesh& other)
      {
        if(this != &other) {
          vertices = other.vertices;
          labels = other.labels;
          type= other.type;
          active = other.active;
          render_id = other.render_id;
          children = other.children;
        }
        return *this;
      }


      Mesh& operator=(Mesh&&) = default;

      std::vector<Geom::Point2d> vertices;
      std::vector<Label> labels;
      MeshType type;
      bool active = true;
      int32_t render_id = -1;
      //std::unordered_map<std::string, Mesh> children; //causes errors with gcc!
      //std::unordered_map<std::string, std::unique_ptr<Mesh>> children;
      std::unordered_map<std::string, Mesh*> children;
    };

    struct MeshGroup
    {
      Mesh parent;
      std::unordered_map<std::string, Mesh> children; 
    };

  public:
    DefaultLayer(AppContext& app_context, const std::string& name);
    ~DefaultLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(double time_step) override;
    void OnImGuiRender() override;
    void SetCanvasSize(float canvas_size);

  private:
    void OnWindowResize(EventWindowResize& e);
    void OnMouseMoved(EventMouseMoved& e);
    void OnMouseScrolled(EventMouseScrolled& e);
    void OnMouseButtonPressed(EventMouseButtonPressed& e);
    void OnMouseButtonReleased(EventMouseButtonReleased& e);

    void Create2DGrid();
    void UpdateCanvas();
    
    void MonotoneAlgoInit();
    void MonotoneAlgoSweepLineUpdate(uint32_t render_id); 

    void GeomTest();

  private:
    Window& m_window;
    GLRenderer& m_renderer;
  #ifdef _WIN32
    GLTextRenderer& m_text_renderer;
  #endif  
    Camera2D& m_camera;
    CameraController2D& m_camera_controller;
    Utils::SpdLogger m_logger;
    bool m_pan_enabled = false;
    float m_canvas_size = 500.0f;

    //std::unordered_map<std::string, MeshGroup> m_mesh_list;
    std::unordered_map<std::string, Mesh> m_mesh_list;
    static std::string s_active_mesh;
    Geom::MonotonePartitionAlgo m_monotone_spawner;
  };

}