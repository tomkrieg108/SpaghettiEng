#include "DefaultLayer.h"

namespace Spg
{
  //namespace fs = std::filesystem;
  using VertexCategory = Geom::MonotonePartitionAlgo::VertexCategory;

  static void PrintDiagPoints(const std::vector<Geom::Point2d> points)
  {
    for(auto& p : points) {
      SPG_TRACE("({},{})", p.x,p.y);
    }
  }

  std::string DefaultLayer::s_active_mesh = "";

  static uint32_t s_montotone_algo_state = 0;

  DefaultLayer::DefaultLayer(AppContext& app_context, const std::string& name) : 
    Layer(app_context, name),
    m_window(app_context.Get<Window>("Window")),
    m_renderer(app_context.Get<GLRenderer>("GLRenderer")),
    m_text_renderer(app_context.Get<GLTextRenderer>("GLTextRenderer")),
    m_camera(app_context.Get<Camera2D>("Camera2D")),
    m_camera_controller(app_context.Get<CameraController2D>("CameraController2D"))
  {
    m_logger = Utils::Logger::Create("Bubble Layer");
    Create2DGrid();
    SetCanvasSize(500.0f);
    //GeomTest();
  }

  void DefaultLayer::SetCanvasSize(float canvas_size)
  {
    m_canvas_size = canvas_size;
    UpdateCanvas();
  }

  void DefaultLayer::MonotoneAlgoSweepLineUpdate(uint32_t render_id)
  { // render_id is the id of the sweep line
    auto& first_event_point = m_monotone_spawner.GetEventQueue().back();
    float sweep_y = first_event_point.vertex->point.y;
    const std::vector<Geom::Point2d> sweep_line_mesh = {glm::vec2(-500.0f, sweep_y),glm::vec2(500.0f, sweep_y) };
    m_renderer.UpdatePosition(render_id, sweep_line_mesh);
  }

  void DefaultLayer::MonotoneAlgoInit()
  {
    SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
    Mesh& mesh = m_mesh_list[s_active_mesh];
    SPG_ASSERT(mesh.type == MeshType::Polygon);

    //clear any child meshes from previous run
    for(auto& item : mesh.children) {
      m_renderer.Delete(item.second.render_id);
    }
    mesh.children.clear();

    //reset the monotone spawner
    m_monotone_spawner.Clear(); //clear DCEL
    auto& vertices = mesh.vertices;
    m_monotone_spawner.Set(vertices); //initialise DCEL
    
    //Add the vertex (points) mesh as child
    Mesh point_mesh;
    point_mesh.vertices = vertices;
    point_mesh.type = MeshType::PointSet;
    point_mesh.active = true;
    point_mesh.render_id = m_renderer.Submit(point_mesh.vertices,glm::vec4(1,1,0,1),GLRenderer::PrimitiveType::Point);
    mesh.children["Points"] = point_mesh;
    
    //set the colours on the polygon vertices based on their category
    //Todo - could update the mooton event to store the index of the original passed in vertex, then use the to supply the correct index in UpdateColour.  This save having to record the unsorted events in the monotone algo and passing those back in GetEventPoints(). Might have been less effot just to implement this thing rather than type out this todo.
    auto& monotone_event_points = m_monotone_spawner.GetEventPoints();
    for(uint32_t i=0; i< point_mesh.vertices.size(); ++i) {
      if(monotone_event_points[i].vertex_category == VertexCategory::Start) {
        m_renderer.UpdateColour(point_mesh.render_id,glm::vec4(0,1,0,1),i); //green
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::End) {
        m_renderer.UpdateColour(point_mesh.render_id,glm::vec4(1,0,0,1),i); //red
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Merge) {
        m_renderer.UpdateColour(point_mesh.render_id,glm::vec4(0,0,1,1),i); //blue
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Split) {
        m_renderer.UpdateColour(point_mesh.render_id,glm::vec4(1,1,1,1),i); //white
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Regular) {
        m_renderer.UpdateColour(point_mesh.render_id,glm::vec4(1,1,0,1),i); //yellow
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Invalid) {
        m_renderer.UpdateColour(point_mesh.render_id,glm::vec4(0.3f,0.3f,0.3f,1),i); //grey
      }
    }

    //Initialise the sweep line
    auto& event_queue = m_monotone_spawner.GetEventQueue();
    auto& first_event_point = event_queue.back();
    float sweep_y = first_event_point.vertex->point.y + 20.0f;
    
    //Add sweep line as a child of polygon mesh
    Mesh sweep_line_mesh;
    sweep_line_mesh.vertices = std::vector{ glm::vec2(-500.0f, sweep_y),  glm::vec2(500.0f, sweep_y)};
    sweep_line_mesh.active;
    sweep_line_mesh.type = MeshType::LineSet;
    sweep_line_mesh.render_id = m_renderer.Submit(sweep_line_mesh.vertices, glm::vec4(0,1,1,1),GLRenderer::PrimitiveType::Line);
    mesh.children["Sweepline"] = sweep_line_mesh;
  }

  void DefaultLayer::OnImGuiRender()
  {
    ImGui::Begin(m_name.c_str());

    if(ImGui::CollapsingHeader("2D Polygon List")) {

      //Parameters for creating a random simple polygon
      ImGui::SeparatorText("Parameters");
      static int32_t max_vertices = 20;
      static float min_edge = 50.0f; 
      static float max_edge = 250.0f; 
      static float min_angle = 20.0f; 
      static float perturb_factor = 0.4f;

      ImGui::SliderInt("Max Vertices", &max_vertices, 8, 50); 
      ImGui::SetItemTooltip("Max number of vertices this polygon can have");

      ImGui::SliderFloat("Min Edge Length", &min_edge, 40.f, 60.f, "%.1f"); 
      ImGui::SetItemTooltip("Min edge length");

      ImGui::SliderFloat("Max Edge Length", &max_edge, 200.f, 300.f, "%.1f"); 
      ImGui::SetItemTooltip("Max edge length");

      ImGui::SliderFloat("Min Angle", &min_angle, 15.0f, 30.0f, "%.1f"); 
      ImGui::SetItemTooltip("Min allowed angle (in degrees) between adjacent edges");

      ImGui::SliderFloat("Perturbation Factor ", &perturb_factor, 0.3f, 0.5f, "%.2f"); 
      ImGui::SetItemTooltip("Adjusts inward/outward movement");

      //Button to create a new random polygon, add it to DefaultLayer::m_polygon
      ImGui::SeparatorText("Create New");
      bool create_clicked = false;
      if(ImGui::Button("Create New")) {
        create_clicked = true;
      }
      ImGui::SetItemTooltip("Create random simple polygon based on parameters set");
      std::vector<Geom::Point2d> poly_points;
      if(create_clicked) {
        Geom::PolygonParameters poly_params{uint32_t(max_vertices*0.7),uint32_t(max_vertices*1.0),min_edge,max_edge,min_angle,perturb_factor};
        Mesh mesh;
        mesh.vertices = Geom::GenerateRandomPolygon_XY(poly_params);
        mesh.type= MeshType::Polygon;
        mesh.active = true;
        std::string mesh_name = std::string("Polygon ") + std::to_string(m_mesh_list.size() + 1);
        m_mesh_list[mesh_name] = mesh;

        //If there's already a mesh active then disable it's display
        if(s_active_mesh != "") {
          //Todo - this code is repeated ofen - refactor
          Mesh& mesh = m_mesh_list[s_active_mesh];
          mesh.active = false;
          m_renderer.Disable(mesh.render_id);
          //also clear any child meshes from this mesh
          for(auto& item : mesh.children) {
            m_renderer.Delete(item.second.render_id);
          }
          mesh.children.clear();

          //reset the monotone algo
          m_monotone_spawner.Clear(); 
          s_montotone_algo_state = 0;
        }
        
        //Update the active mesh to the newly created one
        s_active_mesh = mesh_name;
      }

      //Control for the change the colour of the active mesh
      ImGui::Separator();
      if( s_active_mesh != "") {
        ImGui::Text("Selected: "); 
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());
        Mesh& mesh = m_mesh_list[s_active_mesh];
        if(mesh.render_id == std::numeric_limits<uint32_t>::max())
        {
            mesh.render_id = m_renderer.Submit( mesh.vertices, glm::vec4(0,0,1,1), GLRenderer::PrimitiveType::LineLoop);
        }
        else {
          static ImVec4 seg_color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);
          if(ImGui::ColorEdit3("Seg Color##1", (float*)&seg_color, ImGuiColorEditFlags_Float)) {
            glm::vec4 col{seg_color.x,seg_color.y, seg_color.z, seg_color.w};
            SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
            auto render_id = m_mesh_list[s_active_mesh].render_id;
            for(auto i=0; i<m_mesh_list[s_active_mesh].vertices.size(); ++i) //Todo - not super efficient!!
              m_renderer.UpdateColour(render_id, col,i);
          }
        }
        if(m_mesh_list.empty()) {
          ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "None created yet.");
        }
        ImGui::Separator();

        //List out all the created polygons, save the selected item in s_active_mesh
        for(auto& item : m_mesh_list) {
          if(item.second.type != MeshType::Polygon)
            continue;
          bool selected = (s_active_mesh == item.first);
          char buff[128] = {char(0)};
          std::strncpy(buff, item.first.c_str(), item.first.size());
          if (ImGui::Selectable(buff, selected)) {
            //disable the currently active mesh
            Mesh& mesh = m_mesh_list[s_active_mesh];
            mesh.active = false;
            m_renderer.Disable(mesh.render_id);
            for(auto& item : mesh.children) {
              m_renderer.Delete(item.second.render_id);
            }
            mesh.children.clear();

            //reset the monotone algo
            m_monotone_spawner.Clear(); 
            s_montotone_algo_state = 0;  

            //Update active mesh to the selected item
            s_active_mesh = item.first;
          } 
        }
    
        //Submit selected polygon for rendering
        if(s_active_mesh != "") {
          if(m_mesh_list.find(s_active_mesh) != m_mesh_list.end()) {
            //activate the mewly selected mesh
            Mesh& mesh = m_mesh_list[s_active_mesh];
            m_renderer.Enable(mesh.render_id);
            for(auto& item : mesh.children) {
              m_renderer.Enable(item.second.render_id);
            }
          } 
        }

      }

      ImGui::Separator();

    }

    if(ImGui::CollapsingHeader("2D PointField List")) {
      static int32_t num_points = 20;
      static float radius = 400.0f;
      ImGui::SeparatorText("Point field Parameters");
      ImGui::SliderInt("Num Points", &num_points, 10, 50);
      ImGui::SliderFloat("Radius", &radius, 300, 500);
      ImGui::SeparatorText("Create new point field");
     
      //std::vector<Geom::Point2d> points;
      if(ImGui::Button("Generate")) {
        Mesh mesh;
        mesh.vertices = Geom::GenerateRandomPoints_XY(radius, num_points);
        mesh.type= MeshType::PointSet;
        mesh.active = true;
        mesh.render_id = m_renderer.Submit(mesh.vertices, glm::vec4(1,1,0,1), GLRenderer::PrimitiveType::Point);
        std::string mesh_name = std::string("PointSet ") + std::to_string(m_mesh_list.size() + 1);
        m_mesh_list[mesh_name] = mesh;

        //If there's already a mesh active then disable it's display
        if(s_active_mesh != "") {
          //Todo - this code is repeated ofen - refactor
          Mesh& mesh = m_mesh_list[s_active_mesh];
          mesh.active = false;
          m_renderer.Disable(mesh.render_id);
          //also clear any child meshes from this mesh
          for(auto& item : mesh.children) {
            m_renderer.Delete(item.second.render_id);
          }
          mesh.children.clear();

          //reset the monotone algo
          m_monotone_spawner.Clear(); 
          s_montotone_algo_state = 0;
        }
        //Update the active mesh to the newly created one
        s_active_mesh = mesh_name;
      }

      ImGui::Separator();

      //List out all the created Point Sets, save the selected item in s_active_mesh
      //Todo - repeated
      for(auto& item : m_mesh_list) {
        if(item.second.type != MeshType::PointSet)
          continue;
        bool selected = (s_active_mesh == item.first);
        char buff[128] = {char(0)};
        std::strncpy(buff, item.first.c_str(), item.first.size());
        if (ImGui::Selectable(buff, selected)) {
          //disable the currently active mesh
          Mesh& mesh = m_mesh_list[s_active_mesh];
          mesh.active = false;
          m_renderer.Disable(mesh.render_id);
          for(auto& item : mesh.children) {
            m_renderer.Delete(item.second.render_id);
          }
          mesh.children.clear();

          //reset the monotone algo
          m_monotone_spawner.Clear(); 
          s_montotone_algo_state = 0;  

          //Update active mesh to the selected item
          s_active_mesh = item.first;
        } 
      }

      //Submit selected polygon for rendering
      //todo - repeated
      if(s_active_mesh != "") {
        if(m_mesh_list.find(s_active_mesh) != m_mesh_list.end()) {
          //activet the mewly selected edge
          Mesh& mesh = m_mesh_list[s_active_mesh];
          m_renderer.Enable(mesh.render_id);
          for(auto& item : mesh.children) {
            m_renderer.Enable(item.second.render_id);
          }
        } 
      }
    }

    if(ImGui::CollapsingHeader("Convex Hull 2D")) {
      ImGui::Separator();
      if(s_active_mesh == "")  {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Create a point field.");
      }
      //Todo this triggered if this header opened before any point sets created
      SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
      if(m_mesh_list[s_active_mesh].type != MeshType::PointSet) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Select a point field.");
      }
      else if( s_active_mesh != "") {
        ImGui::Text("Selected: "); 
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());

        SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
        Mesh& mesh = m_mesh_list[s_active_mesh];
        m_renderer.Enable(mesh.render_id); //should be already enabled 
        if(!mesh.children.empty()) {
          ImGui::Text("Convex hull completed");
          for(auto& item : mesh.children) {
            m_renderer.Enable(item.second.render_id);
          }
        }
        else if(ImGui::Button("Run")) {
          Mesh& mesh = m_mesh_list[s_active_mesh];
          Mesh hull_mesh;
          //diagonal_mesh.vertices =  Geom::ConvexHull2D_GiftWrap(mesh.vertices);
          hull_mesh.vertices =  Geom::Convexhull2D_ModifiedGrahams(mesh.vertices);
          hull_mesh.type = MeshType::LineSet;
          hull_mesh.active = true;
          hull_mesh.render_id = m_renderer.Submit( hull_mesh.vertices, glm::vec4(1,1,0,1), GLRenderer::PrimitiveType::LineLoop);
          mesh.children["Diagonals"] = hull_mesh;
        }
      }
    }

    if(ImGui::CollapsingHeader("Polygon Triangulation")) {
      //Todo - this is almost the same as the section above!
      ImGui::Separator();
      if(s_active_mesh == "")  {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Create a polygon to triangulate.");
      }
      SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
      if(m_mesh_list[s_active_mesh].type != MeshType::Polygon) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Select a polygon to triangulate.");
      }
      else if( s_active_mesh != "") {
        ImGui::Text("Selected: "); 
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());

        SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
        Mesh& mesh = m_mesh_list[s_active_mesh];
        m_renderer.Enable(mesh.render_id); //should be already enabled 
        if(!mesh.children.empty()) {
          ImGui::Text("Triangulation completed");
          for(auto& item : mesh.children) {
            m_renderer.Enable(item.second.render_id);
          }
        }
        else if(ImGui::Button("Run")) {
          Mesh& mesh = m_mesh_list[s_active_mesh];
          auto poly_points = mesh.vertices;
          Geom::PolygonSimple polygon = Geom::PolygonSimple(poly_points);

          Mesh diagonal_mesh;
          diagonal_mesh.vertices =  Geom::GenerateEarClipplingDiagonals(&polygon);
          diagonal_mesh.type = MeshType::LineSet;
          diagonal_mesh.active = true;
          diagonal_mesh.render_id = m_renderer.Submit( diagonal_mesh.vertices, glm::vec4(1,1,0,1), GLRenderer::PrimitiveType::Line);
          mesh.children["Diagonals"] = diagonal_mesh;
        }
      }
    }

    if(ImGui::CollapsingHeader("Polygom Monotone partition and triangluation")) {

      bool valid_mesh = false;
      if (s_active_mesh != "") {
        SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
      } 
      Mesh& mesh = m_mesh_list[s_active_mesh];

      if(s_active_mesh == "")  {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Create a polygon to partition.");
      }
      else if((mesh.type != MeshType::Polygon)) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Select a polygon to triangulate.");
      }
      else {
        ImGui::Text("Selected: "); 
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());
        valid_mesh = true;
      }

      if(!valid_mesh) {
        ImGui::End();
        return;
      }
      
      if(s_montotone_algo_state == 0)// pre-init
      {
        if(ImGui::Button("Initialise")) 
        {
          MonotoneAlgoInit();
          s_montotone_algo_state = 1; //post init
        } 
      }

      else if(s_montotone_algo_state == 1) { //post-init
        if(ImGui::Button("Step")) {
          auto render_id = mesh.children["Sweepline"].render_id;
          MonotoneAlgoSweepLineUpdate(render_id); 
          m_monotone_spawner.Step();
          s_montotone_algo_state = 2; //algo running
        }
        ImGui::SameLine();
        if(ImGui::Button("Run")) {
          m_monotone_spawner.MakeMonotone();
          s_montotone_algo_state = 3; //algo complete
        }
      }

      else if(s_montotone_algo_state == 2) { //algo running
        if(m_monotone_spawner.FinishedProcessing()) {
          s_montotone_algo_state = 3; //algo complete
        }
        else if(ImGui::Button("Step")) {
          Mesh& sweep_line_mesh = mesh.children["Sweepline"];
          MonotoneAlgoSweepLineUpdate(sweep_line_mesh.render_id);
          m_monotone_spawner.Step();
          if(m_monotone_spawner.FinishedProcessing()) {
            s_montotone_algo_state = 3; //algo complete
          }
        }
      }

      else if(s_montotone_algo_state == 3) { //algo complete
        ImGui::Text("Algorithm completed");

        if(ImGui::Button("Show Diagonals")) {
          SPG_ASSERT(mesh.children.find("Diagonals") == mesh.children.end());
          Mesh diagonal_mesh;
          diagonal_mesh.vertices = m_monotone_spawner.GetDiagonalEndPoints();
          diagonal_mesh.type = MeshType::LineSet;
          diagonal_mesh.active = true;
          diagonal_mesh.render_id = m_renderer.Submit( diagonal_mesh.vertices, glm::vec4(1,1,0,1), GLRenderer::PrimitiveType::Line);
          mesh.children["Diagonals"] = diagonal_mesh;
          s_montotone_algo_state = 4;
        }
      }

      else if(s_montotone_algo_state == 4) { //Diagonals displayed  
        if(ImGui::Button("Reset")) {
          //m_renderer.Delete( mesh.children["Diagonals"].render_id);
          MonotoneAlgoInit();
          s_montotone_algo_state = 1; //post-init
        }
      }
    }

    ImGui::End();
  }

  void DefaultLayer::UpdateCanvas()
  {
    Camera2D::Params camera_params;
    const float aspect_ratio = m_window.GetAspectRatio();
    camera_params.left = -m_canvas_size*aspect_ratio;
    camera_params.right = m_canvas_size*aspect_ratio;
    camera_params.top = m_canvas_size;
    camera_params.bottom = -m_canvas_size;
    m_camera.SetParams(camera_params);
    m_text_renderer.UpdateView();
  }

  void DefaultLayer::OnUpdate(double delta_time) 
  { 
    m_renderer.Draw(m_camera);
  }

  void DefaultLayer::OnAttach()
  {
    EventManager::AddHandler(this, &DefaultLayer::OnWindowResize);
    EventManager::AddHandler(this, &DefaultLayer::OnMouseScrolled);
    EventManager::AddHandler(this, &DefaultLayer::OnMouseMoved);
    EventManager::AddHandler(this, &DefaultLayer::OnMouseButtonPressed);
    EventManager::AddHandler(this, &DefaultLayer::OnMouseButtonReleased);
  }

  void DefaultLayer::OnDetach()
  {
    //Todo remove handlers
  }

  void DefaultLayer::OnMouseMoved(EventMouseMoved& e)
  {
    if(m_pan_enabled)
    {
      const float factor = -0.2f;
      m_camera_controller.Pan(e.delta_x*factor, e.delta_y*factor);
    }
    e.handled = true;

     m_text_renderer.UpdateView();
  }

  void DefaultLayer::OnMouseScrolled(EventMouseScrolled& e)
  {
    float new_canvas_size = m_canvas_size - e.y_offset*10.0f;
    if(( new_canvas_size > 50.0f) && (new_canvas_size < 5000.0f))
    {
      m_canvas_size = new_canvas_size;
      UpdateCanvas();
    }
    e.handled = true;

     m_text_renderer.UpdateView();
  }

  void DefaultLayer::OnMouseButtonPressed(EventMouseButtonPressed& e)
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

  void DefaultLayer::OnMouseButtonReleased(EventMouseButtonReleased& e)
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

  void DefaultLayer::OnWindowResize(EventWindowResize& e)
  {
    UpdateCanvas();
    e.handled = true;
  }

  void DefaultLayer::Create2DGrid()
  {
    auto grid_data = Geom::GenerateGridMesh_XY(500.0f,50.0f);  
    auto render_id = m_renderer.Submit(grid_data, GLRenderer::PrimitiveType::Line);
  }

  void DefaultLayer::GeomTest()
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

    Geom::DCEL mt_poly = Geom::DCEL(mt_poly_points);
    // mt_poly.Validate();
    // mt_poly.PrintVertices();
    // mt_poly.PrintHalfEdges();
    // mt_poly.PrintFaces();
    Geom::MonotonePartitionAlgo monotone_spawner(mt_poly_points);
    monotone_spawner.MakeMonotone();
    Geom::DCEL& partitioned_polygon = monotone_spawner.GetDCEL();
    partitioned_polygon.PrintFaces();
    partitioned_polygon.PrintVertices();
    partitioned_polygon.PrintHalfEdges();
    
  }

#if 0
  void PolygonRenderData::AddPolygon(const std::string& name, const std::vector<Geom::Point2d>& points, GLShader* shader)
   {
      auto default_point_color = glm::vec4(1,1,0,1);  
      std::vector<PolygonRenderData::Layout> mesh_points;
      for(auto p : points) {
        mesh_points.push_back({{p.x, p.y,-0.1f},default_point_color});
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

      uint32_t size_in_bytes = static_cast<uint32_t>(mesh_points.size() * sizeof(Layout));
      auto vbo_points = GLVertexBuffer(mesh_points.data(), size_in_bytes, PolygonRenderData::s_buffer_layout); 
      uint32_t size_in_bytes_segs = static_cast<uint32_t>(mesh_segs.size() * sizeof(Layout));
      auto vbo_segs = GLVertexBuffer{ mesh_segs.data(), size_in_bytes_segs, PolygonRenderData::s_buffer_layout}; 
      
      Drawable drawable_points(vbo_points,shader,GLRenderer::PrimitiveType::Point);
      Drawable drawable_segs(vbo_segs,shader,GLRenderer::PrimitiveType::Line);
      
      point_drawables[name] = drawable_points;
      seg_drawables[name] = drawable_segs;

      point_meshes[name] = mesh_points;
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
      Drawable drawable_sweep_line(vbo_sweep_line,shader,GLRenderer::PrimitiveType::Line);
      seg_drawables["sweep_line"] = drawable_sweep_line;
    }
    else {
      auto& vbo = seg_drawables["sweep_line"].VAO.GetVertexBuffer();
      vbo.UpdateVertexData(0,size_in_bytes, sweep_line_mesh.data());
    }

    UpdateMonotoneAlgoLabels(name);
  }

  void PolygonRenderData::UpdateMonotoneAlgoLabels(const std::string& name)
  {
    m_labels.clear();
    auto& monotone_event_points = this->monotone_spawner.GetEventPoints();
    auto itr = point_meshes.find(name);
    SPG_ASSERT(itr != point_meshes.end());

    for(auto& event_point : monotone_event_points) {
      Label label;
      label.pos = event_point.vertex->point;
      label.text = std::to_string(event_point.tag);
      m_labels.push_back(label);
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
#endif
}