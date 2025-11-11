#include "Voronoi.h"
#include <iostream>
#include <format>

namespace Geom
{
  Voronoi::Voronoi(std::vector<Geom::Point2d>& points)
  {

  }

  void Voronoi::BuildFiagram(std::vector<Geom::Point2d>& points)
  {
    for(auto& p : points) {
      m_event_queue.push(SiteEvent{p});
    }

    while(!m_event_queue.empty()){
      auto e = m_event_queue.top();
      if(std::holds_alternative<SiteEvent>(e)) {
        HandleSiteEvent(std::get<SiteEvent>(e));
      } 
      else {
        HandleCircleEvent(std::get<CircleEvent>(e));
      }
    }
  }

  void Voronoi::HandleSiteEvent(SiteEvent e)
  {

  }

  void Voronoi::HandleCircleEvent(CircleEvent e)
  {

  }

  //==========================================================================
  //Testing / validation only
  //==========================================================================

  void Voronoi::Test()
  {
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("Voronoi - Test");
    SPG_WARN("-------------------------------------------------------------------------");
    Voronoi voronoi;
    
    //Add a bunch of random values
    const uint32_t NUM_VALS = 40;
    const float MIN_VAL = 0;
    const float MAX_VAL = 100;
    
    std::random_device rd;                         
    std::mt19937 mt(rd()); 
    std::uniform_real_distribution<float> fdist(MIN_VAL, MAX_VAL); 
    
    //add some site events
    for(int i=0; i< 10; i++) {
      Point2d p(fdist(mt),fdist(mt));
      voronoi.m_event_queue.push(SiteEvent{p});
    }

    //add some circle events
    for(int i=0; i< 10; i++) {
      Point2d p(fdist(mt),fdist(mt));
      voronoi.m_event_queue.push(CircleEvent{p});
    }

    while(!voronoi.m_event_queue.empty()) {
      auto e = voronoi.m_event_queue.top();
      if(std::holds_alternative<SiteEvent>(voronoi.m_event_queue.top())) {
        auto e = std::get<SiteEvent>(voronoi.m_event_queue.top());
        std::cout << std::format("Site: ({},{})", e.site.x, e.site.y) << "\n";
      }
      else {
        auto e = std::get<CircleEvent>(voronoi.m_event_queue.top());
        std::cout << std::format("Circle: ({},{})", e.circle_bottom.x, e.circle_bottom.y) << "\n";
      }
      voronoi.m_event_queue.pop();
    }

  }


}