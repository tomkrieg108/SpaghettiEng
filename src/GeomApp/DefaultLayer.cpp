#include "DefaultLayer.h"
#include <random>

namespace Spg
{
  using VertexCategory = Geom::MonotonePartitionAlgo::VertexCategory;

  static void PrintDiagPoints(const std::vector<Geom::Point2d> points)
  {
    for(auto& p : points) {
      SPG_TRACE("({},{})", p.x,p.y);
    }
  }

  DefaultLayer::DefaultLayer(AppContext& app_context, const std::string& name) : 
    Layer(app_context, name),
    m_window(app_context.Get<Window>("Window")),
    m_renderer(app_context.Get<GLRenderer>("GLRenderer")),
  #ifdef _WIN32
    m_text_renderer(app_context.Get<GLTextRenderer>("GLTextRenderer")),
  #endif  
    m_camera(app_context.Get<Camera2D>("Camera2D")),
    m_camera_controller(app_context.Get<CameraController2D>("CameraController2D"))
  {
    m_logger = Utils::Logger::Create("Bubble Layer");
    Create2DGrid();
    SetCanvasSize(500.0f);
    GeomTest();
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

    //clear any child meshes & labels from previous run
    for(auto& item : mesh.children) {
      m_renderer.Delete(item.second->render_id);
    }
    mesh.children.clear();
    mesh.labels.clear();

    //reset the monotone spawner
    m_monotone_spawner.Clear(); //clear DCEL
    auto& vertices = mesh.vertices;
    m_monotone_spawner.Set(vertices); //initialise DCEL
    
    //Add the vertex (points) mesh as child
    Mesh* point_mesh = new Mesh;
    point_mesh->vertices = vertices;
    point_mesh->type = MeshType::PointSet;
    point_mesh->active = true;
    point_mesh->render_id = m_renderer.Submit(point_mesh->vertices,glm::vec4(1,1,0,1),GLRenderer::PrimitiveType::Point);
    //mesh.children["Points"] = std::make_unique<Mesh>(std::move(point_mesh));
    mesh.children["Points"] = point_mesh;
    
    //set the colours on the polygon vertices based on their category
    //Todo - could update the mootone event to store the index of the original passed in vertex, then use the to supply the correct index in UpdateColour.  This save having to record the unsorted events in the monotone algo and passing those back in GetEventPoints(). Might have been less effort just to implement this thing rather than type out this todo.
    auto& monotone_event_points = m_monotone_spawner.GetEventPoints();
    for(uint32_t i=0; i< point_mesh->vertices.size(); ++i) {
      if(monotone_event_points[i].vertex_category == VertexCategory::Start) {
        m_renderer.UpdateColour(point_mesh->render_id,glm::vec4(0,1,0,1),i); //green
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::End) {
        m_renderer.UpdateColour(point_mesh->render_id,glm::vec4(1,0,0,1),i); //red
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Merge) {
        m_renderer.UpdateColour(point_mesh->render_id,glm::vec4(0,0,1,1),i); //blue
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Split) {
        m_renderer.UpdateColour(point_mesh->render_id,glm::vec4(1,1,1,1),i); //white
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Regular) {
        m_renderer.UpdateColour(point_mesh->render_id,glm::vec4(1,1,0,1),i); //yellow
      }
      if(monotone_event_points[i].vertex_category == VertexCategory::Invalid) {
        m_renderer.UpdateColour(point_mesh->render_id,glm::vec4(0.3f,0.3f,0.3f,1),i); //grey
      }
    }

    //Initialise the sweep line
    auto& event_queue = m_monotone_spawner.GetEventQueue();
    auto& first_event_point = event_queue.back();
    float sweep_y = first_event_point.vertex->point.y + 20.0f;
    
    //Add sweep line as a child of polygon mesh
    Mesh* sweep_line_mesh = new Mesh;
    sweep_line_mesh->vertices = std::vector{ glm::vec2(-500.0f, sweep_y),  glm::vec2(500.0f, sweep_y)};
    sweep_line_mesh->active;
    sweep_line_mesh->type = MeshType::LineSet;
    sweep_line_mesh->render_id = m_renderer.Submit(sweep_line_mesh->vertices, glm::vec4(0,1,1,1),GLRenderer::PrimitiveType::Line);
    //mesh.children["Sweepline"] = std::make_unique<Mesh>(std::move(sweep_line_mesh));
    mesh.children["Sweepline"] = sweep_line_mesh;

    //Set up the labels for the poygon vertices
    for(auto& event_point : monotone_event_points) {
      Label label;
      label.pos.x = event_point.vertex->point.x - 25;
      label.pos.y = event_point.vertex->point.y + 15;
      label.text = std::to_string(event_point.tag);
      mesh.labels.push_back(label);
    }
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
  #ifdef _WIN32
    m_text_renderer.UpdateView();
  #endif
  }

  void DefaultLayer::OnUpdate(double delta_time) 
  { 
    //Primitive shape rendering
    m_renderer.Draw(m_camera);
    
    //Text Rendering
  #ifdef _WIN32
    if(m_mesh_list.find(s_active_mesh) == m_mesh_list.end()) {
      m_text_renderer.Render("Hello from TexRenderer!!", -50,-50,0.5f,glm::vec3(0,1,0));
      return;
    }
  

    Mesh& mesh = m_mesh_list[s_active_mesh];
    for(auto& label : mesh.labels) {
      m_text_renderer.Render(label.text, label.pos.x, label.pos.y, 0.35f, {1,1,1});
    }
   #endif 
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
    #ifdef _WIN32
       m_text_renderer.UpdateView();
    #endif 
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

    #ifdef _WIN32
     m_text_renderer.UpdateView();
    #endif
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

//-------------------------------------------------------------------------------
//BSTree
//-------------------------------------------------------------------------------
#if 0
    Geom::BSTree::Test();
#endif
//-------------------------------------------------------------------------------
//RBTree
//-------------------------------------------------------------------------------
#if 0
    Geom::Test_RB_Tree();
#endif

//-------------------------------------------------------------------------------
//RBTreeTraversable
//-------------------------------------------------------------------------------
#if 0
    Geom::Test_Tr_RB_Tree();
#endif

//-------------------------------------------------------------------------------
//RangeTree1D
//-------------------------------------------------------------------------------
#if 0
  Geom::RangeTree1D::Test();
#endif

//-------------------------------------------------------------------------------
//RangeTree2D
//-------------------------------------------------------------------------------
#if 1
  Geom::RangeTree2D::Test();
#endif

//-------------------------------------------------------------------------------
//Voronoi
//-------------------------------------------------------------------------------
#if 0
  Geom::Voronoi::Test();
#endif
//-------------------------------------------------------------------------------
//KDTree
//-------------------------------------------------------------------------------
#if 0
    Geom::KDTree2D::Test();
#endif

//-------------------------------------------------------------------------------
//Intersection Set
//-------------------------------------------------------------------------------
#if 0
    Geom::ItersectSet::IntersectionSet::Test();
#endif

//-------------------------------------------------------------------------------
//DCEL
//--------------------------------------------------------------------------------
#if 0
    Geom::DCEL::Test();
#endif
    //#include <typeindex>
    //#include <typeinfo>

    ///std::type_index type1 = typeid(int);
  }
}