#include "BubbleSoup.h"
#include <SpaghettiEng/SpaghettiEng.h>
#include <Geometry/Geometry.h>
#include <iostream>


//external libs
//should be available because they were linked as public in Geom lib and propagated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
//--------------------------------------------

namespace Spg
{
  namespace fs = std::filesystem;

  Application* CreateApplication()
  {
    return new BubbleSoup("Bubble Soup"s);
  }

  //---------------------------------------------------------------------------------

  BubbleSoup::BubbleSoup(const std::string& title) : 
    Application(title)
  {
    Window& window = m_app_context.Get<Window>("Window");
    glm::vec3 cam_pos = glm::vec3(0.0f,0.0f,-0.5f);
    glm::vec3 look_pos = glm::vec3(0.0f,0.0f,0.0f);
    auto camera = CreateRef<Camera2D>();
    camera->SetPosition(cam_pos);
    camera->LookAt(look_pos);
    auto controller = CreateRef<CameraController2D>(*camera);

    m_app_context.Set("Camera2D", camera);
    m_app_context.Set("CameraController2D", controller);

    BubbleLayer* layer = new BubbleLayer(m_app_context, std::string("Bubble Layer"));
    PushLayer(layer);

    //Todo - compile error
    //SPG_WARN("Camera Pos: {}", position);
    //SPG_WARN("Camera Transform: {}", transform);
    //SPG_WARN("Camera View: {}", view);
  }

  BubbleSoup::~BubbleSoup()
  {
  }

  void BubbleLayer::Create2DGrid()
  {
    auto grid_data = Geom::CreateGrid_XY(500.0f,50.0f);  
    uint32_t size_in_bytes = static_cast<uint32_t>(grid_data.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  
    auto vertex_buffer = GLVertexBuffer{ grid_data.data(), size_in_bytes, layout };  
    m_vao_grid.AddVertexBuffer(vertex_buffer);
  }

  void BubbleLayer::CreatePolygon(uint32_t vertex_count)
  {
    //auto data = Geom::CreateRandPolygon_XY(vertex_count);
    auto points = Geom::GenerateRandomPolygon_XY(vertex_count, 0.2f);
    auto data = Geom::GetMeshFromPoints(points);


    uint32_t size_in_bytes = static_cast<uint32_t>(data.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  
    auto vertex_buffer = GLVertexBuffer{ data.data(), size_in_bytes, layout };  
    m_vao_polygon.AddVertexBuffer(vertex_buffer);
    m_vao_points.AddVertexBuffer(vertex_buffer);
  }

  void BubbleLayer::CreateCircle(uint32_t vertex_count)
  {
    auto data = Geom::CreateCircle_XY(300, vertex_count);
    uint32_t size_in_bytes = static_cast<uint32_t>(data.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  
    auto vertex_buffer = GLVertexBuffer{ data.data(), size_in_bytes, layout };  
    m_vao_circle.AddVertexBuffer(vertex_buffer);
  }

  void BubbleLayer::CreatePoints(uint32_t point_count)
  {
    auto data = Geom::CreateRandomPointMesh_XY(400, point_count);
    uint32_t size_in_bytes = static_cast<uint32_t>(data.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  
    auto vertex_buffer = GLVertexBuffer{ data.data(), size_in_bytes, layout };  
    m_vao_points.AddVertexBuffer(vertex_buffer);
  }

  void BubbleLayer::CreateConvexHull(uint32_t point_count)
  {
    std::vector<Geom::Point2d> points = Geom::CreateRandomPoints_XY(400,point_count);
    //std::vector<Geom::Point2d> hull = Geom::ConvexHull2D_GiftWrap(points);
    //std::vector<Geom::Point2d> hull = Geom::ConvexHull2D_GiftWrap_V2(points);
    std::vector<Geom::Point2d> hull = Geom::Convexhull2D_ModifiedGrahams(points);

    std::vector<float> points_data = Geom::GetMeshFromPoints(points);
    std::vector<float> hull_data = Geom::GetMeshFromPoints(hull);

    uint32_t points_size_in_bytes = static_cast<uint32_t>(points_data.size() * sizeof(float));
    uint32_t hull_size_in_bytes = static_cast<uint32_t>(hull_data.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  

    auto vb_points = GLVertexBuffer{ points_data.data(), points_size_in_bytes, layout };  
    m_vao_points.AddVertexBuffer(vb_points);

    auto vb_hull = GLVertexBuffer{ hull_data.data(), hull_size_in_bytes, layout };  
    m_vao_convex_hull.AddVertexBuffer(vb_hull);
  }

  BubbleLayer::BubbleLayer(AppContext& app_context, const std::string& name) : 
    Layer(app_context, name),
    m_window(app_context.Get<Window>("Window")),
    m_renderer(app_context.Get<GLRenderer>("GLRenderer")),
    m_camera(app_context.Get<Camera2D>("Camera2D")),
    m_camera_controller(app_context.Get<CameraController2D>("CameraController2D"))
  {
    m_logger = Utils::Logger::Create("Bubble Layer");

    GLShaderBuilder shader_builder;
    m_coords_shader = shader_builder.Add(ShaderType::Vertex, "coords.vs").Add(ShaderType::Fragment, "coords.fs").Build("Coords Shader");


    Create2DGrid();
    CreatePolygon(24);
    //CreateCircle(32);
    //CreatePoints(12);
    //CreateConvexHull(30);

    GeomTest();
   
    SetCanvasSize(500.0f);
    auto model = glm::mat4(1.0f);
    m_coords_shader->Bind();
    m_coords_shader->SetUniformMat4f("u_model", model);
    m_coords_shader->Unbind();
  }

  void BubbleLayer::SetCanvasSize(float canvas_size)
  {
    m_canvas_size = canvas_size;
    UpdateCanvas();
  }

  void BubbleLayer::UpdateCanvas()
  {
    Camera2D::Params camera_params;
    const float aspect_ratio = m_window.GetAspectRatio();
    camera_params.left = -m_canvas_size*aspect_ratio;
    camera_params.right = m_canvas_size*aspect_ratio;
    camera_params.top = m_canvas_size;
    camera_params.bottom = -m_canvas_size;
    m_camera.SetParams(camera_params);
    m_coords_shader->Bind();
    m_coords_shader->SetUniformMat4f("u_proj", m_camera.GetProjMatrix());
    m_coords_shader->SetUniformMat4f("u_view", m_camera.GetViewMatrix());
    m_coords_shader->Unbind();
  }

  void BubbleLayer::OnUpdate(double delta_time) 
  {
    m_renderer.DrawLines(m_vao_grid, *m_coords_shader);
    m_renderer.DrawLineLoop(m_vao_polygon, *m_coords_shader);
    //m_renderer.DrawLineLoop(m_vao_circle, *m_coords_shader);
    //m_renderer.DrawLineLoop(m_vao_points, *m_coords_shader);
    m_renderer.DrawPoints(m_vao_points, *m_coords_shader);
    //m_renderer.DrawLineLoop(m_vao_convex_hull, *m_coords_shader);
    //m_renderer.DrawLineStrip(m_vao_convex_hull, *m_coords_shader);
  }

  void BubbleLayer::OnEvent(Event& event)
  {
  }

  void BubbleLayer::OnAttach()
  {
    EventManager::AddHandler(this, &BubbleLayer::OnWindowResize);
    EventManager::AddHandler(this, &BubbleLayer::OnMouseScrolled);
    EventManager::AddHandler(this, &BubbleLayer::OnMouseMoved);
    EventManager::AddHandler(this, &BubbleLayer::OnMouseButtonPressed);
    EventManager::AddHandler(this, &BubbleLayer::OnMouseButtonReleased);
  }

  void BubbleLayer::OnDetach()
  {
    //Todo remove handlers
  }

  void BubbleLayer::OnMouseMoved(EventMouseMoved& e)
  {
    if(m_pan_enabled)
    {
      const float factor = 0.2f;
      m_camera_controller.Pan(e.delta_x*factor, e.delta_y*factor);
      m_coords_shader->Bind();
      m_coords_shader->SetUniformMat4f("u_view", m_camera.GetViewMatrix());
      m_coords_shader->Unbind();

       //LOG_INFO(m_logger,"Panning by ({},{}).  New Pos ({},{})  ",e.delta_x*factor, e.delta_y*factor, m_camera.GetPosition().x, m_camera.GetPosition().y);
    }
    e.handled = true;
  }

  void BubbleLayer::OnMouseScrolled(EventMouseScrolled& e)
  {
    float new_canvas_size = m_canvas_size + e.y_offset*10.0f;
    if(( new_canvas_size > 100.0f) && (new_canvas_size < 2000.0f))
    {
      m_canvas_size = new_canvas_size;
      UpdateCanvas();
      //LOG_INFO(m_logger,"Zooming by {}. New size: {} ",e.y_offset*10.0f, m_canvas_size);
    }
    e.handled = true;
  }

  void BubbleLayer::OnMouseButtonPressed(EventMouseButtonPressed& e)
  {
    if(e.btn == Mouse::ButtonLeft)
      LOG_INFO(m_logger,"Left mouse btn presssed");
    if(e.btn == Mouse::ButtonRight)
      LOG_INFO(m_logger,"Right mouse btn presssed");  
    if(e.btn == Mouse::ButtonMiddle)
    {
      m_pan_enabled = true;
      m_window.SetCursorEnabled(false); 
    }
    e.handled = true;
  }

  void BubbleLayer::OnMouseButtonReleased(EventMouseButtonReleased& e)
  {
    if(e.btn == Mouse::ButtonLeft)
      LOG_INFO(m_logger,"Left mouse btn released");
    if(e.btn == Mouse::ButtonRight)
      LOG_INFO(m_logger,"Right mouse btn released");  
    if(e.btn == Mouse::ButtonMiddle)
    {
      m_pan_enabled = false;
      m_window.SetCursorEnabled(true);
    }
    e.handled = true;  
  }

  void BubbleLayer::OnWindowResize(EventWindowResize& e)
  {
    UpdateCanvas();
    e.handled = true;
  }

  void BubbleLayer::OnImGuiRender()
  {
    ImGui::Begin(m_name.c_str());
    if (ImGui::CollapsingHeader("Gui For this layer"))
    {
      ImGui::Text("Data and controls for this layer goes here");
    }
    ImGui::End();
  }

  void BubbleLayer::GeomTest()
  {
    //Test out some geom stuff
    Geom::LineSeg2D seg1(Geom::Point2d{0,0}, Geom::Point2d{10,0});
    Geom::LineSeg2D seg2(Geom::Point2d{0,0}, Geom::Point2d{10,10});
    Geom::LineSeg2D seg3(Geom::Point2d{0,0}, Geom::Point2d{0,10});
    Geom::LineSeg2D seg4(Geom::Point2d{0,0}, Geom::Point2d{-10,10});
    Geom::LineSeg2D seg5(Geom::Point2d{0,0}, Geom::Point2d{-10,0});
    Geom::LineSeg2D seg6(Geom::Point2d{0,0}, Geom::Point2d{-10,-10});
    Geom::LineSeg2D seg7(Geom::Point2d{0,0}, Geom::Point2d{0,-10});
    Geom::LineSeg2D seg8(Geom::Point2d{0,0}, Geom::Point2d{10,-10});
    LOG_WARN(m_logger, "{} {} {} {} {} {} {} {}", seg1.Angle(),seg2.Angle(),seg3.Angle(),seg4.Angle(),seg5.Angle(),seg6.Angle(),seg7.Angle(),seg8.Angle());

    //angle difference
    seg1.Set({-10,-10},{-40,10});
    seg2.Set({-10,-10},{15,5});
    auto angle = Geom::Angle_LineSeg2D_LineSeg2d(seg1,seg2);
    LOG_WARN(m_logger, "Angle between seg 1 and seg 2: {} ", angle);

    //area
    Geom::Point2d p1{13.16,-47.2};
    Geom::Point2d p2{42.98,-26.07};
    Geom::Point2d p3{-13.52,-38.96};
    auto area1 =Geom::SignedAreaTriangle2d(p1,p2,p3);
    auto area2 =Geom::SignedAreaTriangle2d(p3,p1,p2);
    auto area3 =Geom::SignedAreaTriangle2d(p2,p3,p1);
    LOG_WARN(m_logger, "Signed Area of Triangle p1,p2,p3: {} {} {} ", area1, area2, area3);

    auto area4 =Geom::SignedAreaTriangle2d(p1,p3,p2);
    auto area5 =Geom::SignedAreaTriangle2d(p2,p1,p3);
    auto area6 =Geom::SignedAreaTriangle2d(p3,p2,p1);
    LOG_WARN(m_logger, "Signed Area of Triangle p1,p2,p3: {} {} {} ", area4, area5, area6);
  
    //orientation test
    p1 = {-4,-2};
    p2 = {2,1};
    p3 = {-3,3};
    auto orient = Geom::Orientation2d(p1,p2,p3);  //Left
    orient = Geom::Orientation2d(p1,p3,p2);  //Right

    p1 = {-4,-4};
    p2 = {4,4};
    p3 = {-2,-2};
    orient = Geom::Orientation2d(p1,p2,p3);  //Between
    orient = Geom::Orientation2d(p3,p2,p1);  //Behind
    orient = Geom::Orientation2d(p1,p3,p2);  //beyond
    orient = Geom::Orientation2d(p1,p2,p1);  //origin
    orient = Geom::Orientation2d(p1,p2,p2);  //destination
    //-----------------------------------------

    //equality test
    float f1 = 1.123004;
    float f2 = 1.123005;
    bool eq = Geom::Equal(f1,f2);
    eq = Geom::Equal(f2,f1);

    p1 = {1.0001f, 2.0002f};
    p2 = {1.0002f, 2.0001f};
    eq = Geom::Equal(p1,p2);
    eq = Geom::Equal(p2,p1);

    //------------------------------------------------------------------------------
    //Intersection test
    p1 = {-8.8, 2.88}; //intersect
    //p1 = {-2.94, 1.39}; //no intersect
    p2 = {8.2, -3.1};
    p3 = {-9.27, -6.49};
    Geom::Point2d p4 = {-1.69,6.4};
    Geom::Point2d intersection(0,0);

    seg1 = {p1,p2};
    seg2 = {p3,p4};

    bool intersect = Geom::LineSegs2dIntersect(seg1,seg2);
    if(intersect)
    {
      Geom::GetLines2dIntersectPoint(seg1,seg2, intersection);
    }
  }

  void AppPrintHello()
  {
    std::cout << "Hello From App\n";
    
    #ifdef SPG_DEBUG
      std::cout << "APP: SPG_DEBUG defined\n";
    #elif defined(SPG_RELEASE)
        std::cout << "APP: SPG_RELEASE defined\n";
    #endif  

    #ifdef _WIN32
      std::cout << "APP: WIN platform\n";
    #elif defined(__linux__)
      std::cout << "APP: LINUX platform\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "APP: x86_64 architecture\n";
    #endif

    std::cout << "External libs linked into App exe:\n";
    std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << "\n";
    std::cout << "FMT:" << FMT_VERSION << "\n";
    std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << "\n";
    std::cout << "SPDLOG....:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";
    std::cout << "\n";

    std::cout << "####################################################\n\n";
  }
}

int main()
{
  Spg::Application::SystemInit();
#ifdef SPG_LIB_LINK_CHECK 
  Utils::LibCheck();
  Geom::GeomLibHello();
  Spg::AppPrintHello();
#endif

  auto app = Spg::CreateApplication();
  app->Run();
  delete app; //Doesn't get called in release mode!
  return 0;
}