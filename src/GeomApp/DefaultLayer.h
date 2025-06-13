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
    
    struct Mesh
    {
      std::vector<Geom::Point2d> vertices;
      MeshType type;
      bool active = true;
      uint32_t render_id = std::numeric_limits<uint32_t>::max(); //initially undefined
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
    GLTextRenderer& m_text_renderer;
    Camera2D& m_camera;
    CameraController2D& m_camera_controller;
    Utils::SpdLogger m_logger;
    bool m_pan_enabled = false;
    float m_canvas_size = 500.0f;

    std::unordered_map<std::string, Mesh> m_mesh_list;
    static std::string s_active_mesh;
    Geom::MonotonePartitionAlgo m_monotone_spawner;
  };

}