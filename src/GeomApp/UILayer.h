#pragma once
#include <string>
#include <unordered_map>

#include "SpaghettiEng/SpaghettiEng.h"
#include "MathLib/MathLib.h"
#include "Geometry/Geometry.h"

#include "GeomApp/DefaultLayer.h"
#include "SpaghettiEng/Core/ImguiLayer.h"
/*
  {} []
*/

//Note:  This is all very hacky!!

class Geom::MonotonePartitionAlgo;

namespace Spg
{
  class ServiceLocator;
  class DefaultLayer;

  using Mesh =  DefaultLayer::Mesh;
  using MeshType = DefaultLayer::MeshType;
  using MeshGroup = DefaultLayer::MeshGroup;
  

  class UILayer : public ImguiLayer
  {
    public:
      
      UILayer(DefaultLayer* default_layer, ServiceLocator& service_locator, const std::string& name);
      ~UILayer() = default;

    private:
      void Draw(double delta_time) override;   

      //Refs to all the stuff from default layer - HACK!!
      DefaultLayer* m_default_layer;
      GLSimpleRenderer& m_renderer;
      std::unordered_map<std::string, Mesh>& m_mesh_list;
      Geom::MonotonePartitionAlgo& m_monotone_spawner;
      std::string& m_active_mesh;
  };

}