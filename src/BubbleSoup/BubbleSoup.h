#pragma once
#include <SpaghettiEng/SpaghettiEng.h>
#include <SpaghettiEng/Camera/Camera2D.h>
#include <SpaghettiEng/OpenGL32/GL32Renderer.h>

using namespace std::string_literals;

namespace Spg
{
  class BubbleLayer : public Layer
  {
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
    void CreatePolygon(uint32_t vertex_count);
    void CreateCircle(uint32_t vertex_count);
    void CreatePoints(uint32_t point_count);
    void CreateConvexHull(uint32_t point_count);
    void UpdateCanvas();

    void GeomTest();
    
  private:
    Window& m_window;
    GLRenderer& m_renderer;
    Camera2D& m_camera;
    CameraController2D& m_camera_controller;
    Utils::SpdLogger m_logger;
    
    Scope<GLShader> m_coords_shader = nullptr;
    bool m_pan_enabled = false;
    float m_canvas_size = 500.0f;

    GLVertexArray m_vao_grid;
    GLVertexArray m_vao_polygon;
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
