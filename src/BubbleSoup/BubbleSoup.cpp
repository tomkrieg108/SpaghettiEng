#include <iostream>

#include <SpaghettiEng/SpaghettiEng.h>
#include <Geometry/Geometry.h>
#include "BubbleSoup.h"

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
    m_renderer.Draw(m_vao_grid, *m_coords_shader);
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