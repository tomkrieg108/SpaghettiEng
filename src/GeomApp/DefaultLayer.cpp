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
    m_text_renderer(app_context.Get<GLTextRenderer>("GLTextRenderer")),
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
      m_renderer.Delete(item.second.render_id);
    }
    mesh.children.clear();
    mesh.labels.clear();

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
    //Todo - could update the mootone event to store the index of the original passed in vertex, then use the to supply the correct index in UpdateColour.  This save having to record the unsorted events in the monotone algo and passing those back in GetEventPoints(). Might have been less effort just to implement this thing rather than type out this todo.
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
    m_text_renderer.UpdateView();
  }

  void DefaultLayer::OnUpdate(double delta_time) 
  { 
    //Primitive shape rendering
    m_renderer.Draw(m_camera);
    
    //Text Rendering
    if(m_mesh_list.find(s_active_mesh) == m_mesh_list.end()) {
      m_text_renderer.Render("Hello from TexRenderer!!", -50,-50,0.5f,glm::vec3(0,1,0));
      return;
    }

    Mesh& mesh = m_mesh_list[s_active_mesh];
    for(auto& label : mesh.labels) {
      m_text_renderer.Render(label.text, label.pos.x, label.pos.y, 0.35f, {1,1,1});
    }
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
//-------------------------------------------------------------------------------
//BSTree
//-------------------------------------------------------------------------------
#if 0
    LOG_WARN(m_logger,"--------------------------");
    LOG_WARN(m_logger,"BST TREE (NEW)");
    LOG_WARN(m_logger,"--------------------------");
    std::vector<float> values{2,11,4,125,15,3,9,32,71,43,27,1};
    Geom::BSTree bstree(values);
    std::vector<float> op_vals = bstree.InOrderTraverse();
    for(auto val : op_vals) {
      LOG_TRACE(m_logger, "{},", val);
    } 

    LOG_TRACE(m_logger, "Min is {} ", bstree.Min());
    LOG_TRACE(m_logger, "Max is {} ", bstree.Max());
    LOG_TRACE(m_logger, "Size is {} ", bstree.Size());

    LOG_TRACE(m_logger, "Contains 15 ? {} ", bstree.Contains(15));
    LOG_TRACE(m_logger, "Contains 9 ? {} ", bstree.Contains(9));
    LOG_TRACE(m_logger, "Contains 32 ? {} ", bstree.Contains(32));
    LOG_TRACE(m_logger, "Contains 77 ? {} ", bstree.Contains(77));
    LOG_TRACE(m_logger, "Contains 39 ? {} ", bstree.Contains(39));
    LOG_TRACE(m_logger, "Contains 127 ? {} ", bstree.Contains(127));

    LOG_TRACE(m_logger, "Sucessor of 1 is {} ", bstree.Next(1));
    LOG_TRACE(m_logger, "Sucessor of 2 is {} ", bstree.Next(2));
    LOG_TRACE(m_logger, "Sucessor of 3 is {} ", bstree.Next(3));
    LOG_TRACE(m_logger, "Sucessor of 4 is {} ", bstree.Next(4));
    LOG_TRACE(m_logger, "Sucessor of 9 is {} ", bstree.Next(9));
    LOG_TRACE(m_logger, "Sucessor of 11 is {} ", bstree.Next(11));
    LOG_TRACE(m_logger, "Sucessor of 15 is {} ", bstree.Next(15));
    LOG_TRACE(m_logger, "Sucessor of 27 is {} ", bstree.Next(27));
    LOG_TRACE(m_logger, "Sucessor of 32 is {} ", bstree.Next(32));
    LOG_TRACE(m_logger, "Sucessor of 43 is {} ", bstree.Next(43));
    LOG_TRACE(m_logger, "Sucessor of 71 is {} ", bstree.Next(71));
    LOG_TRACE(m_logger, "Sucessor of 125 is {} ", bstree.Next(125));

    LOG_TRACE(m_logger, "Predecessor of 1 is {} ", bstree.Previous(1));
    LOG_TRACE(m_logger, "Predecessor of 2 is {} ", bstree.Previous(2));
    LOG_TRACE(m_logger, "Predecessor of 3 is {} ", bstree.Previous(3));
    LOG_TRACE(m_logger, "Predecessor of 4 is {} ", bstree.Previous(4));
    LOG_TRACE(m_logger, "Predecessor of 9 is {} ", bstree.Previous(9));
    LOG_TRACE(m_logger, "Predecessor of 11 is {} ", bstree.Previous(11));
    LOG_TRACE(m_logger, "Predecessor of 15 is {} ", bstree.Previous(15));
    LOG_TRACE(m_logger, "Predecessor of 27 is {} ", bstree.Previous(27));
    LOG_TRACE(m_logger, "Predecessor of 32 is {} ", bstree.Previous(32));
    LOG_TRACE(m_logger, "Predecessor of 43 is {} ", bstree.Previous(43));
    LOG_TRACE(m_logger, "Predecessor of 71 is {} ", bstree.Previous(71));
    LOG_TRACE(m_logger, "Predecessor of 125 is {} ", bstree.Previous(125));

    LOG_TRACE(m_logger, "Contains 2 ? {} ", bstree.Contains(2));
    bstree.Erase(2);
    LOG_TRACE(m_logger, "Contains 2 ? {} ", bstree.Contains(2));

    LOG_TRACE(m_logger, "Contains 32 ? {} ", bstree.Contains(32));
    bstree.Erase(32);
    LOG_TRACE(m_logger, "Contains 32 ? {} ", bstree.Contains(32));

    LOG_TRACE(m_logger, "Contains 11 ? {} ", bstree.Contains(11));
    bstree.Erase(11);
    LOG_TRACE(m_logger, "Contains 11 ? {} ", bstree.Contains(11));

    LOG_TRACE(m_logger, "Contains 15 ? {} ", bstree.Contains(15));
    bstree.Erase(15);
    LOG_TRACE(m_logger, "Contains 15 ? {} ", bstree.Contains(15));

    LOG_TRACE(m_logger, "Contains 15 ? {} ", bstree.Contains(15));
    bstree.Erase(15);
    LOG_TRACE(m_logger, "Contains 15 ? {} ", bstree.Contains(15));

    LOG_TRACE(m_logger, "Contains 71 ? {} ", bstree.Contains(71));
    bstree.Erase(71);
    LOG_TRACE(m_logger, "Contains 71 ? {} ", bstree.Contains(71));

    op_vals = bstree.InOrderTraverse();
    for(auto val : op_vals) {
      LOG_TRACE(m_logger, "{},", val);
    } 
#endif

//-------------------------------------------------------------------------------
//RBTree V3 (templated)
//-------------------------------------------------------------------------------
#if 0
     
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("Red-Black Tree V3 (Templated)");
    SPG_WARN("-------------------------------------------------------------------------");
    {
      
      std::vector<std::pair<const int,int>> map_vals {{2,200},{11,1100},{4,400},{125,12500},{15,1500},{3,300},{9,900},{32,3200},{71,7100},{43,4300},{27,2700},{1,100}};

      Geom::Map<int, int> my_map(map_vals);
      my_map.Validate();

      //Traversal and iterating
      {
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse");
        my_map.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        
        // SPG_INFO("Ranged for loop traverse");
        // for(auto& [key,val] : my_map) {
        //   SPG_TRACE("[{},{}]", key, val);
        // }

        SPG_INFO("Loop using Iterator")
        for(auto itr = my_map.begin(); itr != my_map.end(); ++itr) {
          auto& [key,val] = *itr;
          SPG_TRACE("[{},{}]", key, val);
        }
      }

      //Find(), Contains()
      {
        for(auto& [key, value]: map_vals) {
          SPG_INFO("Contains {}? {}", key, my_map.Contains(key));
        }
        SPG_INFO("Contains {}? {}", -15, my_map.Contains(-15));
        SPG_INFO("Contains {}? {}", 0, my_map.Contains(0));
        SPG_INFO("Contains {}? {}", 160, my_map.Contains(160));
        SPG_INFO("Contains {}? {}", 33, my_map.Contains(33));
      }

      //LowerBound() UpperBound()
      {
        for(auto& [key, value]: map_vals) {
          auto itr = my_map.LowerBound(key);
          if(itr == my_map.end()) {
            SPG_INFO("LowerBound {}? end", key);
          }
          else {
            SPG_INFO("LowerBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_map.LowerBound(k);
          if(itr == my_map.end()) {
            SPG_INFO("LowerBound {}? end", k);
          }
          else {
            SPG_INFO("LowerBound {}? {}", k, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          auto itr = my_map.UpperBound(key);
          if(itr == my_map.end()) {
            SPG_INFO("UpperBound {}? end", key);
          }
          else {
            SPG_INFO("UpperBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_map.UpperBound(k);
          if(itr == my_map.end()) {
            SPG_INFO("UpperBound {}? end", k);
          }
          else {
            SPG_INFO("UpperBound {}? {}", k, (*itr).first);
          }
        }
        
      }

      //Deletion
      {
        my_map.Erase(1);
        my_map.Erase(71);
        my_map.Erase(27);
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse After erase");
        my_map.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        my_map.Validate();
      }

      //Stress test with large data, insertion, erasure, clear
      {
        my_map.Clear();
        //Add a bunch of random values
        std::vector<int> rb_values;
        std::vector<bool> rb_values_to_delete;
      
        const uint32_t RB_NUM_VALS = 10000;
        const int RB_MIN_VAL = -100000;
        const int RB_MAX_VAL = 100000;
    
        std::random_device rd;                         
        std::mt19937 mt(rd()); 
        std::uniform_int_distribution<int> dist(RB_MIN_VAL, RB_MAX_VAL); 
      
        for(int i=0; i< RB_NUM_VALS; i++) {
          int val = dist(mt);
          auto element = std::make_pair(val,val*100);
          if(my_map.Insert(element)) {
            rb_values.push_back(val);
            rb_values_to_delete.push_back(false);    
          }
        }
        my_map.Validate();

        //pick values to delete at random
        std::uniform_int_distribution<int> idist(0, rb_values.size()-1);  
        const uint32_t RB_NUM_VALS_TO_DEL = rb_values.size()/2;
        for(int i=0; i<RB_NUM_VALS_TO_DEL; i++ ) {
          int idx = 0;
          do {
            idx = idist(mt);
          } while (rb_values_to_delete[idx] == true);
          rb_values_to_delete[idx] = true;
        }
        
        SPG_INFO("Deleting {} values at random ", RB_NUM_VALS_TO_DEL);
        for(int i=0; i<rb_values_to_delete.size(); i++ ) {
          if(!rb_values_to_delete[i])
            continue;
          my_map.Erase(rb_values[i]);
        }
        my_map.Validate();
        SPG_INFO("Clearing Tree");
        my_map.Clear();
        my_map.Validate();
        SPG_INFO("Tree Size {}: ", my_map.Size());

        SPG_INFO("Add a few more elements");
        for(auto& element : map_vals)
          my_map.Insert(element);
        SPG_INFO("Ranged for loop traverse");
        for(auto& [key,val] : my_map) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("Tree Size {}: ", my_map.Size());
        my_map.Validate();
      }
    
      //Copy move constructor, assignment operator
      {
        auto t2 = my_map; //copy constructor
        Geom::Map<int,int> t3;
        t3 = t2; //copy assignment;
        SPG_INFO("t2:");
        for(auto& [key,val] : t2) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t3:");
        for(auto& [key,val] : t3) {
          SPG_TRACE("[{},{}]", key, val);
        }
        auto t4 = std::move(t2); //move ctr
        Geom::Map<int,int> t5;
        t5 = std::move(t3); //move assignment

        SPG_INFO("After move:");
        SPG_INFO("t2:");
        for(auto& [key,val] : t2) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t3:");
        for(auto& [key,val] : t3) {
          SPG_TRACE("[{},{}]", key, val);
        }

        SPG_INFO("t4:");
        for(auto& [key,val] : t4) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t5:");
        for(auto& [key,val] : t5) {
          SPG_TRACE("[{},{}]", key, val);
        }
      }
    }
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
//KDTree
//-------------------------------------------------------------------------------
#if 0
     std::vector<Geom::Point2d> kd_values;

    //Add a bunch of random values
    const uint32_t KD_NUM_VALS = 1000;
    const float KD_MIN_VAL = 0;
    const float KD_MAX_VAL = 200;
    
    std::random_device rd;                         
    std::mt19937 mt(rd()); 
    std::uniform_real_distribution<float> fdist(KD_MIN_VAL, KD_MAX_VAL); 
    
    for(int i=0; i< KD_NUM_VALS; i++) {
      Geom::Point2d p(fdist(mt),fdist(mt));
      kd_values.push_back(p);
    }

    Geom::KDTree2D kdtree(kd_values);
    Geom::KDTree2D kdtree2(std::move(kd_values));

    auto points1 = kdtree.CollectAllPoints();
    auto points2 = kdtree2.CollectAllPoints();

    Geom::KDTree2D::Range range{20,80,20,80};
    auto points3 = kdtree.RangeSearch(range);
    auto points4 = kdtree2.RangeSearch(range);

    kdtree.ValidateSearch(range);
#endif

//-------------------------------------------------------------------------------
//Intersection Set
//-------------------------------------------------------------------------------
#if 0    
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
#endif

//-------------------------------------------------------------------------------
//DCEL
//--------------------------------------------------------------------------------
#if 0    
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

    // Geom::DCEL mt_poly = Geom::DCEL(mt_poly_points);
    // Geom::MonotonePartitionAlgo monotone_spawner(mt_poly_points);
    // monotone_spawner.MakeMonotone();
    // Geom::DCEL& partitioned_polygon = monotone_spawner.GetDCEL();
    // partitioned_polygon.PrintFaces();
    // partitioned_polygon.PrintVertices();
    // partitioned_polygon.PrintHalfEdges();
#endif
    //#include <typeindex>
    //#include <typeinfo>

    ///std::type_index type1 = typeid(int);
  }
}