#include "DefaultLayer.h"

namespace Spg
{
  //Todo - This is something of a dogs breakfast.

  std::string DefaultLayer::s_active_mesh = "";
  static uint32_t s_montotone_algo_state = 0;
  
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
            m_renderer.Delete(item.second->render_id);
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
      if( s_active_mesh != "") {
        ImGui::Text("Selected: "); 
        ImGui::SameLine();
      #ifdef _WIN32 
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());
      #endif
        Mesh& mesh = m_mesh_list[s_active_mesh];
        if(mesh.render_id == std::numeric_limits<uint32_t>::max())
        {
            mesh.render_id = m_renderer.Submit( mesh.vertices, glm::vec4(0,0,1,1), GLRenderer::PrimitiveType::LineLoop);
        }
        else {
          //Control for the change the colour of the active mesh
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
              m_renderer.Delete(item.second->render_id);
            }
            mesh.children.clear();
            mesh.labels.clear();

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
              m_renderer.Enable(item.second->render_id);
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
      ImGui::SliderInt("Num Points", &num_points, 3, 50);
      ImGui::SliderFloat("Radius", &radius, 100, 500);
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
            m_renderer.Delete(item.second->render_id);
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
            m_renderer.Delete(item.second->render_id);
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
            m_renderer.Enable(item.second->render_id);
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
      #ifdef _WIN32  
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());
      #endif  

        SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
        Mesh& mesh = m_mesh_list[s_active_mesh];
        m_renderer.Enable(mesh.render_id); //should be already enabled 
        if(!mesh.children.empty()) {
          ImGui::Text("Convex hull completed");
          for(auto& item : mesh.children) {
            m_renderer.Enable(item.second->render_id);
          }
        }
        else if(ImGui::Button("Run")) {
          Mesh& mesh = m_mesh_list[s_active_mesh];
          Mesh* hull_mesh = new Mesh;
          //diagonal_mesh.vertices =  Geom::ConvexHull2D_GiftWrap(mesh.vertices);
          hull_mesh->vertices =  Geom::Convexhull2D_ModifiedGrahams(mesh.vertices);
          hull_mesh->type = MeshType::LineSet;
          hull_mesh->active = true;
          hull_mesh->render_id = m_renderer.Submit( hull_mesh->vertices, glm::vec4(1,1,0,1), GLRenderer::PrimitiveType::LineLoop);
          mesh.children["Hull"] = hull_mesh;
        }
      }
    }

     if(ImGui::CollapsingHeader("Voronoi")) {
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
      #ifdef _WIN32  
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());
      #endif  

        SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
        Mesh& mesh = m_mesh_list[s_active_mesh];
        m_renderer.Enable(mesh.render_id); //should be already enabled 
        if(!mesh.children.empty()) {
          ImGui::Text("Voronoi Algo Complete");
          for(auto& item : mesh.children) {
            m_renderer.Enable(item.second->render_id);
          }
        }
        else if(ImGui::Button("Run")) {
          Mesh& mesh = m_mesh_list[s_active_mesh];

          Geom::Voronoi_V4::Voronoi voronoi(mesh.vertices);
          voronoi.Construct();
          
          Mesh* verts_mesh = new Mesh;
          //diagonal_mesh.vertices =  Geom::ConvexHull2D_GiftWrap(mesh.vertices);
          verts_mesh->vertices =  voronoi.GetVertexPoints();
          verts_mesh->type = MeshType::PointSet;
          verts_mesh->active = true;
          verts_mesh->render_id = m_renderer.Submit( verts_mesh->vertices, glm::vec4(1,0,0,1), GLRenderer::PrimitiveType::Point);
          mesh.children["VoronoiVerts"] = verts_mesh;

          Mesh* edges_mesh = new Mesh;
          edges_mesh->vertices = voronoi.GetConnectedEdgePoints();
          edges_mesh->type = MeshType::LineSet;
          edges_mesh->active = true;
          edges_mesh->render_id = m_renderer.Submit( edges_mesh->vertices, glm::vec4(0,0,1,1), GLRenderer::PrimitiveType::Line);
          mesh.children["VoronoiEdges"] = edges_mesh;

          Mesh* loose_edges_mesh = new Mesh;
          loose_edges_mesh->vertices = voronoi.GetLooseEdgePoints();
          loose_edges_mesh->type = MeshType::LineSet;
          loose_edges_mesh->active = true;
          loose_edges_mesh->render_id = m_renderer.Submit( loose_edges_mesh->vertices, glm::vec4(0,1,1,1), GLRenderer::PrimitiveType::Line);
          mesh.children["VoronoiLooseEdges"] = loose_edges_mesh;
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
      #ifdef _WIN32  
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());
      #endif  

        SPG_ASSERT(m_mesh_list.find(s_active_mesh) != m_mesh_list.end());
        Mesh& mesh = m_mesh_list[s_active_mesh];
        m_renderer.Enable(mesh.render_id); //should be already enabled 
        if(!mesh.children.empty()) {
          ImGui::Text("Triangulation completed");
          for(auto& item : mesh.children) {
            m_renderer.Enable(item.second->render_id);
          }
        }
        else if(ImGui::Button("Run")) {
          Mesh& mesh = m_mesh_list[s_active_mesh];
          auto poly_points = mesh.vertices;
          Geom::PolygonSimple polygon = Geom::PolygonSimple(poly_points);

          Mesh* diagonal_mesh = new Mesh;
          diagonal_mesh->vertices =  Geom::GenerateEarClipplingDiagonals(&polygon);
          diagonal_mesh->type = MeshType::LineSet;
          diagonal_mesh->active = true;
          diagonal_mesh->render_id = m_renderer.Submit( diagonal_mesh->vertices, glm::vec4(1,1,0,1), GLRenderer::PrimitiveType::Line);
          //mesh.children["Diagonals"] = std::make_unique<Mesh>(std::move(diagonal_mesh));
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
      #ifdef _WIN32
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s_active_mesh.c_str());
      #endif
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
          auto render_id = mesh.children["Sweepline"]->render_id;
          MonotoneAlgoSweepLineUpdate(render_id); 
          m_monotone_spawner.Step();
          s_montotone_algo_state = 2; //algo running
        }
        ImGui::SameLine();
        if(ImGui::Button("Run")) {
          //m_monotone_spawner.GetDCEL().Validate();
          m_monotone_spawner.MakeMonotone();
          s_montotone_algo_state = 3; //algo complete
        }
      }

      else if(s_montotone_algo_state == 2) { //algo running
        if(m_monotone_spawner.FinishedProcessing()) {
          s_montotone_algo_state = 3; //algo complete
        }
        else if(ImGui::Button("Step")) {
          Mesh& sweep_line_mesh = *(mesh.children["Sweepline"]);
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
          Mesh* diagonal_mesh = new Mesh;
          diagonal_mesh->vertices = m_monotone_spawner.GetMonotonDiagonals();
          diagonal_mesh->type = MeshType::LineSet;
          diagonal_mesh->active = true;
          diagonal_mesh->render_id = m_renderer.Submit( diagonal_mesh->vertices, glm::vec4(1,1,0,1),      GLRenderer::PrimitiveType::Line);
          //mesh.children["Diagonals"] = std::make_unique<Mesh>(std::move(diagonal_mesh));
          mesh.children["Diagonals"] = diagonal_mesh;
          s_montotone_algo_state = 4;
        }
      }

      else if(s_montotone_algo_state == 4) { //Diagonals displayed  
        if(ImGui::Button("Reset")) {
          //m_monotone_spawner.GetDCEL().Validate();
          MonotoneAlgoInit();
          s_montotone_algo_state = 1; //post-init
        }

        else if(ImGui::Button("Triangulate")) {
          //m_monotone_spawner.GetDCEL().Validate();
          m_monotone_spawner.Triangulate(); //Good luck!
          s_montotone_algo_state = 5; //Triangulation complete
        }
      }

      else if (s_montotone_algo_state == 5) {
        ImGui::Text("Triangulation completed");
        if(ImGui::Button("Show Triabgulation")) {
          Mesh* trianglulation_diag_mesh = new Mesh;
          trianglulation_diag_mesh->vertices = m_monotone_spawner.GetTriangulationDiagonals();
          trianglulation_diag_mesh->type = MeshType::LineSet;
          trianglulation_diag_mesh->active = true;
          trianglulation_diag_mesh->render_id = m_renderer.Submit( trianglulation_diag_mesh->vertices, glm::vec4(1,1,1,1),GLRenderer::PrimitiveType::Line);
          //mesh.children["TriDiagonals"] = std::make_unique<Mesh>(std::move(trianglulation_diag_mesh));
          mesh.children["TriDiagonals"] = trianglulation_diag_mesh;
          s_montotone_algo_state = 4;
        }
      }
    }

    ImGui::End();
  }

}