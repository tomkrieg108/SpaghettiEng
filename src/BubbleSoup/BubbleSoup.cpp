#include "BubbleSoup.h"
#include <SpaghettiEng/SpaghettiEng.h>


//external libs
//should be available because they were linked as public in Geom lib and propagated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <array>
//--------------------------------------------

namespace Spg
{
  namespace fs = std::filesystem;
  using PolygonRenderData = BubbleLayer::PolygonRenderData;
  using MonotoneSpawner = Geom::Monotone_V2::MonotoneSpawner5000;
  using DCEL_Polygon = Geom::DCEL::Polygon;

  static BufferLayout s_layout =
  {
    {"a_position", ShaderDataType::Float3},
    {"a_color", ShaderDataType::Float4}
  };  

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
  }

  BubbleSoup::~BubbleSoup()
  {
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
    m_shader = shader_builder.Add(ShaderType::Vertex, "basic.vs").Add(ShaderType::Fragment, "basic.fs").Build("Basic Shader");




    Create2DGrid();
    //CreatePoints(40);
    //CreateCircle(32);
    //CreatePolygon(18);
    //CreateTriangulatedPolygon(30);
    CreateMonotonePartitionedPolygon(24);
    //CreateConvexHull(30);

    GeomTest();
   
    SetCanvasSize(500.0f);
    auto model = glm::mat4(1.0f);
    m_shader->Bind();
    m_shader->SetUniformMat4f("u_model", model);
    m_shader->Unbind();
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
    m_shader->Bind();
    m_shader->SetUniformMat4f("u_proj", m_camera.GetProjMatrix());
    m_shader->SetUniformMat4f("u_view", m_camera.GetViewMatrix());
    m_shader->Unbind();
  }

  void BubbleLayer::OnUpdate(double delta_time) 
  {
    for(auto& element : m_drawables) {
      auto& drawable = element.second;
      if(drawable.enabled) {
        m_renderer.Draw(drawable.VAO, *(drawable.shader), drawable.draw_mode);
      }
    }
    for(auto& element : m_polygon_render_data.point_drawables) {
      auto& drawable = element.second;
      if(drawable.enabled) {
        m_renderer.Draw(drawable.VAO, *(drawable.shader), drawable.draw_mode);
      }
    }
    for(auto& element : m_polygon_render_data.seg_drawables) {
      auto& drawable = element.second;
      if(drawable.enabled) {
        m_renderer.Draw(drawable.VAO, *(drawable.shader), drawable.draw_mode);
      }
    }
    //m_renderer.DrawPoints(m_vao_points, *m_shader);
    //m_renderer.DrawLineLoop(m_vao_circle, *m_coords_shader);

    //m_renderer.DrawLineLoop(m_vao_polygon, *m_shader);
    //m_renderer.DrawLines(m_vao_polygon_diagonals, *m_shader);

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
      const float factor = -0.2f;
      m_camera_controller.Pan(e.delta_x*factor, e.delta_y*factor);
      m_shader->Bind();
      m_shader->SetUniformMat4f("u_view", m_camera.GetViewMatrix());
      m_shader->Unbind();
    }
    e.handled = true;
  }

  void BubbleLayer::OnMouseScrolled(EventMouseScrolled& e)
  {
    float new_canvas_size = m_canvas_size - e.y_offset*10.0f;
    if(( new_canvas_size > 50.0f) && (new_canvas_size < 5000.0f))
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

    if (ImGui::CollapsingHeader("Polygons")) {

      static std::string s_selected_entry = "";
      /*
        s_montotone_algo_state applies to the polygon corresponding to s_selected_entry
        0 => pre-init, 1 => post-init, 2 => algo running, 3 => algo complete (partition data ready)
        Todo - make this a static member of RenderData thing, alsong with methods to reset, step etc
      */
      static uint32_t s_montotone_algo_state = 0; 

      //if (ImGui::TreeNode((void*)(intptr_t)1, "Generate")) {  
        ImGui::SeparatorText("Parameters");
        static int32_t max_vertices = 20;
        static float min_edge = 50.0f; 
        static float max_edge = 250.0f; 
        static float min_angle = 20.0f; 
        static float perturb_factor = 0.4f;
        ImGui::SliderInt("Max Vertices", &max_vertices, 8, 50); ImGui::SetItemTooltip("Max number of vertices this polygon can have");
        ImGui::SliderFloat("Min Edge Length", &min_edge, 40.f, 60.f, "%.1f"); ImGui::SetItemTooltip("Min edge length");
        ImGui::SliderFloat("Max Edge Length", &max_edge, 200.f, 300.f, "%.1f"); ImGui::SetItemTooltip("Max edge length");
        ImGui::SliderFloat("Min Angle", &min_angle, 15.0f, 30.0f, "%.1f"); ImGui::SetItemTooltip("Min allowed angle (in degrees) between adjacent edges");
        ImGui::SliderFloat("Perturbation Factor ", &perturb_factor, 0.3f, 0.5f, "%.2f"); ImGui::SetItemTooltip("Adjusts inward/outward movement");

        ImGui::SeparatorText("Create New");
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
        bool generate_clicked = false;
        if(ImGui::Button("Generate")) {
          generate_clicked = true;
        }
        ImGui::PopStyleColor(3);
        ImGui::SetItemTooltip("Generate new simple polygon using parameters");

        ImGui::SameLine();
        ImGui::Text("Name: ");
        ImGui::SameLine();
        static char buff[128] = {char(0)};
        if(buff[0] == '\0') {
          std::string name = std::string("Polygon ") + std::to_string(m_polygon_render_data.NumPolygons()+1);
          std::strncpy(buff, name.c_str(), name.size());
        }
        ImGui::InputText("##Name", buff, sizeof(buff));
      
        if(generate_clicked) {
          Geom::PolygonParameters poly_params{uint32_t(max_vertices*0.7),uint32_t(max_vertices*1.0),min_edge,max_edge,min_angle,perturb_factor};
          s_selected_entry = std::string(buff);
          auto poly_points = Geom::GenerateRandomPolygon_XY(poly_params);
          m_polygon_render_data.AddPolygon(s_selected_entry,poly_points,m_shader.get());
          std::string name = std::string("Polygon ") + std::to_string(m_polygon_render_data.NumPolygons()+1);
          std::strncpy(buff, name.c_str(), name.size());
          s_montotone_algo_state = 0; //pre-init
          m_polygon_render_data.monotone_spawner.Clear();
        }

       // ImGui::TreePop();
      //}
      
      //if (ImGui::TreeNode((void*)(intptr_t)2, "Select")) {  
        for(auto& item : m_polygon_render_data.point_meshes) {
          bool selected = (s_selected_entry == item.first);
          auto itr = m_polygon_render_data.point_drawables.find(item.first);
          SPG_ASSERT(itr != m_polygon_render_data.point_drawables.end());
          m_polygon_render_data.seg_drawables[item.first].enabled = selected;
          itr->second.enabled = selected;
          char buff[128] = {char(0)};
          std::strncpy(buff, item.first.c_str(), item.first.size());
          if (ImGui::Selectable(buff, selected)) {
            s_selected_entry = item.first;
            s_montotone_algo_state = 0; //pre-init
            m_polygon_render_data.monotone_spawner.Clear();
          } 
        }
       //ImGui::TreePop();
      //} 

        ImGui::Separator();
        if( s_selected_entry != "") {
          ImGui::Text("Selected: "); 
          ImGui::SameLine();
          ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_selected_entry.c_str());
        }
        ImGui::Separator();
      
      //if (ImGui::TreeNode((void*)(intptr_t)3, "Colours")) {

        static ImVec4 point_color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);
        static ImVec4 seg_color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);
      
        if( s_selected_entry != "") {
          if(ImGui::ColorEdit3("Point Color##1", (float*)&point_color, ImGuiColorEditFlags_Float)) {
            glm::vec4 col{point_color.x,point_color.y, point_color.z, point_color.w};
            m_polygon_render_data.SetPointColor(s_selected_entry, col);
          }
          if(ImGui::ColorEdit3("Seg Color##1", (float*)&seg_color, ImGuiColorEditFlags_Float)) {
            glm::vec4 col{seg_color.x,seg_color.y, seg_color.z, seg_color.w};
            m_polygon_render_data.SetSegColor(s_selected_entry, col);
          }
        }

        ImGui::SeparatorText("Monotone Partitioning");
        if( s_selected_entry != "") {
          ImGui::Text("Selected: "); 
          ImGui::SameLine();
          ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_selected_entry.c_str());
          ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.7f, 0.7f));
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
          
          if(s_montotone_algo_state == 0) { //pre-init
            if(ImGui::Button("Initialise")) {
              auto& points = m_polygon_render_data.GetPolygonPoints(s_selected_entry);
              m_polygon_render_data.monotone_spawner.Set(points);
              m_polygon_render_data.InitialiseMonotoneAlgoVisuals(s_selected_entry,m_shader.get());
              m_polygon_render_data.seg_drawables["sweep_line"].enabled = true;
              s_montotone_algo_state = 1; //post-init
            }
          }

          else if(s_montotone_algo_state == 1) { //post-init
            if(ImGui::Button("Step")) {
              m_polygon_render_data.UpdateMonotoneAlgoSweepLineDisplay();
              m_polygon_render_data.monotone_spawner.Step();
              s_montotone_algo_state = 2; //algo running
            }
            ImGui::SameLine();
            if(ImGui::Button("Run")) {
            m_polygon_render_data.monotone_spawner.MakeMonotone();
            s_montotone_algo_state = 3; //algo complete
            }
          }

          else if(s_montotone_algo_state == 2) { //algo running
            if(m_polygon_render_data.monotone_spawner.FinishedProcessing()) {
              s_montotone_algo_state = 3; //algo complete
            }
            else if(ImGui::Button("Step")) {
              m_polygon_render_data.UpdateMonotoneAlgoSweepLineDisplay();
              m_polygon_render_data.monotone_spawner.Step();
            }
          }

          else if(s_montotone_algo_state == 3) { //algo complete
            ImGui::Text("Algorithm completed");
            if(ImGui::Button("Reset")) {
              m_polygon_render_data.monotone_spawner.Clear();
              auto& points = m_polygon_render_data.GetPolygonPoints(s_selected_entry);
              m_polygon_render_data.monotone_spawner.Set(points);
              m_polygon_render_data.InitialiseMonotoneAlgoVisuals(s_selected_entry,m_shader.get());
              m_polygon_render_data.seg_drawables["sweep_line"].enabled = true;
              s_montotone_algo_state = 1; //post-init
            }
          } 

          ImGui::PopStyleColor(3);
        }

        //ImGui::TreePop();
      //} 
      
    }

    ImGui::End();
  }

  void BubbleLayer::Create2DGrid()
  {
    auto grid_data = Geom::GenerateGridMesh_XY(500.0f,50.0f);  
    uint32_t size_in_bytes = static_cast<uint32_t>(grid_data.size() * sizeof(float));
    auto vertex_buffer = GLVertexBuffer{ grid_data.data(), size_in_bytes, s_layout };  
    m_vao_grid.AddVertexBuffer(vertex_buffer);
    Drawable drawable;
    drawable.draw_mode = GLRenderer::DrawMode::Line;
    drawable.shader = m_shader.get();
    drawable.VAO.AddVertexBuffer(vertex_buffer);
    drawable.enabled = true;
    m_drawables["grid"] = drawable;
  }

  void BubbleLayer::CreateTriangulatedPolygon(uint32_t vertex_count)
  {
    float perturbFactor = 0.4; // Adjusts inward/outward movement
    float minEdge = 50.0; // Minimum edge length
    float maxEdge = 250.0; // Maximum edge length
    float minAngle = 30.0; // Minimum allowed angle (in degrees)

    Geom::PolygonParameters params{(uint32_t)(vertex_count*0.7f), vertex_count,minEdge,maxEdge,minAngle,perturbFactor };

    auto polygon_points = Geom::GenerateRandomPolygon_XY(params);

    Geom::PolygonSimple polygon = Geom::PolygonSimple(polygon_points);
    auto diagonal_points = Geom::GenerateEarClipplingDiagonals(&polygon);
    
    auto poly_mesh =  Geom::GetMeshFromPoints(polygon_points, glm::vec4(1,1,0,1));
    auto diagonals_mesh = Geom::GetMeshFromPoints(diagonal_points, glm::vec4(0,0,1,1));
    uint32_t poly_size_in_bytes = static_cast<uint32_t>(poly_mesh.size() * sizeof(float));
    uint32_t diagonal_size_in_bytes = static_cast<uint32_t>(diagonals_mesh.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  
    auto poly_buffer = GLVertexBuffer{ poly_mesh.data(), poly_size_in_bytes, layout };   
    auto diagonals_buffer = GLVertexBuffer{ diagonals_mesh.data(), diagonal_size_in_bytes, layout };  

    m_vao_polygon.AddVertexBuffer(poly_buffer);
    m_vao_polygon_diagonals.AddVertexBuffer(diagonals_buffer);
  }

  void BubbleLayer::CreateMonotonePartitionedPolygon(uint32_t vertex_count)
  {
    float perturbFactor = 0.4; // Adjusts inward/outward movement
    float minEdge = 50.0; // Minimum edge length
    float maxEdge = 250.0; // Maximum edge length
    float minAngle = 30.0; // Minimum allowed angle (in degrees)

    Geom::PolygonParameters params{(uint32_t)(vertex_count*0.7f), vertex_count,minEdge,maxEdge,minAngle,perturbFactor };
    auto polygon_points = Geom::GenerateRandomPolygon_XY(params);

    //Identical to the above function apart from the next 2 lines!
    DCEL_Polygon polygon = DCEL_Polygon(polygon_points);
    
    auto diagonal_points1 = Geom::GenerateMonotoneDiagonals(&polygon);
    MonotoneSpawner monotone_spawner(polygon_points);

    monotone_spawner.MakeMonotone();
    auto diagonal_points = monotone_spawner.GetDiagonalEndPoints();
    //SPG_ASSERT(diagonal_points1.size() == diagonal_points.size());

    auto poly_mesh =  Geom::GetMeshFromPoints(polygon_points, glm::vec4(1,1,0,1));
    auto diagonals_mesh = Geom::GetMeshFromPoints(diagonal_points1, glm::vec4(0,0,1,1));
    uint32_t poly_size_in_bytes = static_cast<uint32_t>(poly_mesh.size() * sizeof(float));
    uint32_t diagonal_size_in_bytes = static_cast<uint32_t>(diagonals_mesh.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  
    auto poly_buffer = GLVertexBuffer{ poly_mesh.data(), poly_size_in_bytes, layout };   
    auto diagonals_buffer = GLVertexBuffer{ diagonals_mesh.data(), diagonal_size_in_bytes, layout };  

    m_vao_polygon.AddVertexBuffer(poly_buffer);
    m_vao_polygon_diagonals.AddVertexBuffer(diagonals_buffer);
  }

  void BubbleLayer::CreateCircle(uint32_t vertex_count)
  {
    auto points = Geom::GenerateCircle_XY(400,vertex_count);
    auto data = Geom::GetMeshFromPoints(points,glm::vec4(0,1,0,1));
    uint32_t size_in_bytes = static_cast<uint32_t>(data.size() * sizeof(float));

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };  
    auto vertex_buffer = GLVertexBuffer{ data.data(), size_in_bytes, layout };  
    m_vao_circle.AddVertexBuffer(vertex_buffer);
    m_vao_points.AddVertexBuffer(vertex_buffer);
  }

  void BubbleLayer::CreatePoints(uint32_t point_count)
  {
    auto points = Geom::GenerateRandomPoints_XY(400, point_count);
    auto data = Geom::GetMeshFromPoints(points,glm::vec4(1,1,0,1));

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
    std::vector<Geom::Point2d> points = Geom::GenerateRandomPoints_XY(400,point_count);
    //std::vector<Geom::Point2d> hull = Geom::ConvexHull2D_GiftWrap(points);
    std::vector<Geom::Point2d> hull = Geom::Convexhull2D_ModifiedGrahams(points);

    std::vector<float> points_data = Geom::GetMeshFromPoints(points, glm::vec4(1,1,0,1));
    std::vector<float> hull_data = Geom::GetMeshFromPoints(hull,glm::vec4(0,1,0,1));

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

  void BubbleLayer::GeomTest()
  {
   #if 1
    //Test out some geom stuff
    Geom::LineSeg2D seg1(Geom::Point2d{0,0}, Geom::Point2d{10,0});
    Geom::LineSeg2D seg2(Geom::Point2d{0,0}, Geom::Point2d{10,10});
    //angle difference
    seg1.Set({-10,-10},{-40,10});
    seg2.Set({-10,-10},{15,5});
    auto angle = Geom::ComputeAngleInDegrees(seg1,seg2);
    LOG_WARN(m_logger, "Angle between seg 1 and seg 2: {} ", angle);

    //area
    Geom::Point2d p1{13.16,-47.2};
    Geom::Point2d p2{42.98,-26.07};
    Geom::Point2d p3{-13.52,-38.96};
    auto area1 =Geom::ComputeSignedArea(p1,p2,p3);
    auto area2 =Geom::ComputeSignedArea(p3,p1,p2);
    auto area3 =Geom::ComputeSignedArea(p2,p3,p1);
    LOG_WARN(m_logger, "Signed Area of Triangle p1,p2,p3: {} {} {} ", area1, area2, area3);

    auto area4 =Geom::ComputeSignedArea(p1,p3,p2);
    auto area5 =Geom::ComputeSignedArea(p2,p1,p3);
    auto area6 =Geom::ComputeSignedArea(p3,p2,p1);
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

    bool intersect = Geom::IntersectionExists(seg1,seg2);
    if(intersect)
    {
      Geom::ComputeIntersection(seg1,seg2, intersection);
    }

    //-----------------------------------------------------------------------
    //BST
    std::vector<float> values{2,11,4,125,15,3,9,32,71,43,27};
    std::list<float> list;
    Geom::BST bst(values);
    bst.InOrderTraverse(bst.GetRootNode(), list);
    for(auto val : list) {
      LOG_TRACE(m_logger, "{},", val);
    }
    float predecessor,sucessor;
    auto success = bst.Predecessor(15,predecessor);
    LOG_TRACE(m_logger, "Predecessor of 15 is {} ", predecessor);
    success = bst.Successor(32,sucessor);
    LOG_TRACE(m_logger, "Sucessor of 32 is {} ", sucessor);

    success = bst.Predecessor(2,predecessor);
    if(success) {
      LOG_TRACE(m_logger, "predecessor of 2 is {} ", predecessor);
    }
    else {
      LOG_TRACE(m_logger, "2 does not have a predecessor");   
    }

    success = bst.Successor(125,sucessor);
    if(success) {
      LOG_TRACE(m_logger, "Sucessor of 125 is {} ", sucessor);
    }
    else {
      LOG_TRACE(m_logger, "125 does not have a successor");   
    }


    bst.Delete(32.0);
    list.clear();
    bst.InOrderTraverse(bst.GetRootNode(), list);
    LOG_TRACE(m_logger, "Delete 32");   
    for(auto val : list) {
      LOG_TRACE(m_logger, "{},", val);
    }

    bst.Delete(99.0);
    list.clear();
    bst.InOrderTraverse(bst.GetRootNode(), list);
    LOG_TRACE(m_logger, "Delete 99");   
    for(auto val : list) {
      LOG_TRACE(m_logger, "{},", val);
    }

    #endif

    //-------------------------------------------------------------------------------
    //Intersection Set
    LOG_WARN(m_logger, "-----------------------------------");  
    LOG_TRACE(m_logger, "iNTERSECTION TESTING");  
    std::vector<Geom::LineSeg2D> segs 
    {
      {{-1,4},{-2,1}}, //f
      {{-2,12},{2,-2}}, //g
      {{0,2},{-4,6}}, //h
      {{-2,6},{2,10}}, //i
      {{6,4},{2,10}}, //j
      {{4,4},{2,10}}, //k
      {{2,10},{4,14}}, //l
      {{8,10},{-4,10}}, //m horizontal line - 
      //{{8,9},{-4,10}}, //m near horizontal line - 

     
       {{-4,6},{-4,10}}, 
       {{4,4},{6,4}}, //p horizontal line
       
       {{-8,8},{-4,8}}, //r horizontal line
       {{-6,12},{-6,4}}, //q vertical line
       {{6,14},{6,10}}, //q vertical line
       //{{6,14},{6,9.1666667}}, //q vertical line

    };
   
    // seg1 = {{-4,10},{8,10}};
    // seg2 = {{-4,10},{8,10}};
    // eq = Geom::Equal(seg1.start, seg2.start) && Geom::Equal(seg1.end, seg2.end);

    Geom::ItersectSet::IntersectionSet intersection_set{segs};
    intersection_set.Process();

    //-------------------------------------------------------------------------------
    //DCEL
    //----------------------------------------------------------------------------------
    //MONOTON PARTITION
    std::vector<Geom::Point2d> mt_poly_points =
    {
      {16.42f,12.51f},  //A 1
      {13.95,10.36},    //B 2
      {11.2,18.4},      //C 3
      {9.2,16.4},       //D 4
      {6.6,17.8},       //E 5
      {4,16},           //F 6
      {6.62,13.16},     //G 7
      {5.52,9.06},      //H 8
      {3.38,11.36},     //I 9
      {2.54,6.49},      //J 10
      {6.04,3.49},      //K 11
      {8.99,5.24},      //L 12
      {12,2},           //M 13
      {12.26,7.79},     //N 14
      {17.04,6.99}      //O 15
    };

    Geom::DCEL_V2::DCEL mt_poly = Geom::DCEL_V2::DCEL(mt_poly_points);
    // mt_poly.Validate();
    // mt_poly.PrintVertices();
    // mt_poly.PrintHalfEdges();
    // mt_poly.PrintFaces();
    Geom::Monotone_V2::MonotoneSpawner5000 monotone_spawner(mt_poly_points);
    monotone_spawner.MakeMonotone();
    Geom::DCEL_V2::DCEL& partitioned_polygon = monotone_spawner.GetDCEL();
    partitioned_polygon.PrintFaces();
    partitioned_polygon.PrintVertices();
    partitioned_polygon.PrintHalfEdges();
    
  }

  BufferLayout PolygonRenderData::s_buffer_layout = 
  {
    {"a_position", ShaderDataType::Float3},
    {"a_color", ShaderDataType::Float4}
  };

  void PolygonRenderData::AddPolygon(const std::string& name, const std::vector<Geom::Point2d>& points, GLShader* shader)
   {
      auto default_point_color = glm::vec4(1,1,0,1);  
      std::vector<PolygonRenderData::Layout> mesh;
      for(auto p : points) {
        mesh.push_back({{p.x, p.y,-0.1f},default_point_color});
      }
      
      auto default_seg_color = glm::vec4(0.5f,0.5f,0,1);  
      std::vector<PolygonRenderData::Layout> mesh_segs;
      for(uint32_t i=0; i<points.size(); ++i) {
        uint32_t i_next = (i==points.size()-1) ? 0 : i+1;
        glm::vec3 p_cur = {points[i].x, points[i].y, 0.1f};
        glm::vec3 p_next = {points[i_next].x, points[i_next].y, -0.1f};
        mesh_segs.push_back({p_cur, default_seg_color});
        mesh_segs.push_back({p_next, default_seg_color});
      }

      uint32_t size_in_bytes = static_cast<uint32_t>(mesh.size() * sizeof(Layout));
      auto vbo_points = GLVertexBuffer{ mesh.data(), size_in_bytes, PolygonRenderData::s_buffer_layout}; 
      uint32_t size_in_bytes_segs = static_cast<uint32_t>(mesh_segs.size() * sizeof(Layout));
      auto vbo_segs = GLVertexBuffer{ mesh_segs.data(), size_in_bytes_segs, PolygonRenderData::s_buffer_layout}; 
      
      Drawable drawable_points(vbo_points,shader,GLRenderer::DrawMode::Point);
      Drawable drawable_segs(vbo_segs,shader,GLRenderer::DrawMode::Line);
      
      point_drawables[name] = drawable_points;
      seg_drawables[name] = drawable_segs;

      point_meshes[name] = mesh;
      seg_meshes[name] = mesh_segs;

      polygon_points[name] = points; 

      //Stuff below for figuring out a compiler error - can be deleted
      //todo - don't seem to get these compile errors when doing the same thing in Visual Studio 22! with c++ 17/20
      //auto dcel = monotone_spawner.GetDCEL();
      //DCEL_Polygon d1 = dcel; //error
      //Geom::DCEL::Polygon d2 = std::move(dcel); // error
      // auto pp = points;
      // DCEL_Polygon p1 = DCEL_Polygon(pp); //ok
      // DCEL_Polygon p3 = p1; // ok
      //DCEL_Polygon p2 = DCEL_Polygon(points); //error
      // std::vector<DCEL_Polygon> poly_vec;
      // poly_vec.push_back(p1); //ok
      //poly_vec.push_back(dcel); //error
   }

  void PolygonRenderData::InitialiseMonotoneAlgoVisuals(const std::string& name, GLShader* shader)
  {
    auto& monotone_event_points = this->monotone_spawner.GetEventPoints();
    auto itr = point_meshes.find(name);
    SPG_ASSERT(itr != point_meshes.end());
    auto& point_mesh = itr->second;
    SPG_ASSERT(monotone_event_points.size() == point_mesh.size());
    for(uint32_t i=0; i< point_mesh.size(); ++i) {
      if(monotone_event_points[i].vertex_category == Geom::Monotone_V2::VertexCategory::Start) {
        SetPointColor(name,i,glm::vec4(0,1,0,1)); //green
      }
      if(monotone_event_points[i].vertex_category == Geom::Monotone_V2::VertexCategory::End) {
        SetPointColor(name,i,glm::vec4(1,0,0,1)); //red
      }
      if(monotone_event_points[i].vertex_category == Geom::Monotone_V2::VertexCategory::Merge) {
        SetPointColor(name,i,glm::vec4(0,0,1,1)); //blue
      }
      if(monotone_event_points[i].vertex_category == Geom::Monotone_V2::VertexCategory::Split) {
        SetPointColor(name,i,glm::vec4(1,1,1,1)); //white
      }
      if(monotone_event_points[i].vertex_category == Geom::Monotone_V2::VertexCategory::Regular) {
         SetPointColor(name,i,glm::vec4(1,1,0,1)); //yellow
      }
      if(monotone_event_points[i].vertex_category == Geom::Monotone_V2::VertexCategory::Invalid) {
        SetPointColor(name,i,glm::vec4(0.3f,0.3f,0.3f,1)); //gray
      }
    }

    auto& event_queue = this->monotone_spawner.GetEventQueue();
    auto& first_event_point = event_queue.back();
    float sweep_y = first_event_point.vertex->point.y + 20.0f;
    std::array<Layout,2> sweep_line_mesh;
    sweep_line_mesh[0].pos = glm::vec3(-500.0f, sweep_y, 0.1f);
    sweep_line_mesh[1].pos = glm::vec3(500.0f, sweep_y, 0.1f);
    sweep_line_mesh[0].col = sweep_line_mesh[1].col = {0.f,0.f,0.8f,1};
    uint32_t size_in_bytes = static_cast<uint32_t>(sweep_line_mesh.size() * sizeof(Layout));
    if(seg_drawables.find("sweep_line") == seg_drawables.end()) {
      auto vbo_sweep_line = GLVertexBuffer{ sweep_line_mesh.data(), size_in_bytes, PolygonRenderData::s_buffer_layout};
      Drawable drawable_sweep_line(vbo_sweep_line,shader,GLRenderer::DrawMode::Line);
      seg_drawables["sweep_line"] = drawable_sweep_line;
    }
    else {
      auto& vbo = seg_drawables["sweep_line"].VAO.GetVertexBuffer();
      vbo.UpdateVertexData(0,size_in_bytes, sweep_line_mesh.data());
    }

    //todo - setup drawable etc for active edges in status structure
    std::vector<PolygonRenderData::Layout> mesh_active_edges;

   

   
  } 

  void PolygonRenderData::UpdateMonotoneAlgoSweepLineDisplay()
  {
    auto& event_queue = this->monotone_spawner.GetEventQueue();
    auto& first_event_point = event_queue.back();
    float sweep_y = first_event_point.vertex->point.y;
    std::array<Layout,2> sweep_line_mesh;
    sweep_line_mesh[0].pos = glm::vec3(-500.0f, sweep_y, 0.1f);
    sweep_line_mesh[1].pos = glm::vec3(500.0f, sweep_y, 0.1f);
    sweep_line_mesh[0].col = sweep_line_mesh[1].col = {0.f,0.f,0.8f,1};
    uint32_t size_in_bytes = static_cast<uint32_t>(sweep_line_mesh.size() * sizeof(Layout));
    SPG_ASSERT(seg_drawables.find("sweep_line") != seg_drawables.end())
    auto& vbo = seg_drawables["sweep_line"].VAO.GetVertexBuffer();
    vbo.UpdateVertexData(0,size_in_bytes, sweep_line_mesh.data());
  }

  void PolygonRenderData::UpdateMonotoneAlgoActiveEdgeDisplay()
  {
    std::vector<Layout> active_edges;
    auto& status_structure = this->monotone_spawner.GetStatusStructure();
    for(auto& item : status_structure) {
      auto& edge = item.first;
      auto& helper = item.second;
      Layout point1, point2;
      point1.pos = glm::vec3(edge.origin->point, -0.2f);
      point2.pos = glm::vec3(edge.twin->origin->point, -0.2f);
      point1.col = point2.col = glm::vec4(1,1,0,1);
    }
  }

  void PolygonRenderData::SetPointColor(const std::string& name, uint32_t index, const glm::vec4& color)
  {
    auto itr = point_meshes.find(name);
    SPG_ASSERT(itr != point_meshes.end());
    auto& point_mesh = itr->second;
    SPG_ASSERT(index < point_mesh.size());
    
    auto itr2 = point_drawables.find(name);
    SPG_ASSERT(itr2 != point_drawables.end());
    auto& point_drawable = itr2->second;
    auto& vbo = point_drawable.VAO.GetVertexBuffer();

    PolygonRenderData::Layout new_data;
    new_data.col = color;
    new_data.pos =  point_mesh[index].pos;

    vbo.UpdateVertexData(index, sizeof(new_data), (void *)(&new_data));
  }

  void PolygonRenderData::SetPointColor(const std::string& name, const glm::vec4& color)
  {
    auto itr = point_meshes.find(name);
    SPG_ASSERT(itr != point_meshes.end());
    auto& point_mesh = itr->second;

    for(uint32_t i=0; i<point_mesh.size(); ++i) {
      SetPointColor(name,i,color);
    }
  }

  void PolygonRenderData::SetPointColor(const std::string& name, const std::vector<uint32_t>& indicies, const glm::vec4& color)
  {
    
  }

  void PolygonRenderData::SetSegColor(const std::string& name, uint32_t index, const glm::vec4& color)
  {
    uint32_t seg_idx = index*2;

    auto itr = seg_meshes.find(name);
    SPG_ASSERT(itr != seg_meshes.end());
    auto& seg_mesh = itr->second;
    SPG_ASSERT(seg_idx+1 < seg_mesh.size()); 

    auto itr2 = seg_drawables.find(name);
    SPG_ASSERT(itr2 != seg_drawables.end());
    auto& seg_drawable = itr2->second;
    auto& vbo = seg_drawable.VAO.GetVertexBuffer();

    PolygonRenderData::Layout new_data[2];
    new_data[0].col = new_data[1].col = color;
    new_data[0].pos = seg_mesh[seg_idx].pos;
    new_data[1].pos = seg_mesh[seg_idx+1].pos;

    vbo.UpdateVertexData(seg_idx, sizeof(new_data), (void *)(&new_data));
  }

  void PolygonRenderData::SetSegColor(const std::string& name, const std::vector<uint32_t>& indicies, const glm::vec4& color)
  {
    
  }

  void PolygonRenderData::SetSegColor(const std::string& name, const glm::vec4& color)
  {
     auto itr = point_meshes.find(name);
    SPG_ASSERT(itr != point_meshes.end());
    auto& point_mesh = itr->second;

    for(uint32_t i=0; i<point_mesh.size(); ++i) {
      SetSegColor(name,i,color);
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
}