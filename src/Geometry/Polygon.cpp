#include "Geometry/Polygon.h"

#include "CoreLib/Core.h"
#include "Geometry/GeomUtils.h"

namespace Geom
{
  namespace SP
  {
    Polygon::Polygon(const std::vector<Geom::Point2d>& points)
    {
      const auto size = points.size();
      SPG_ASSERT(size >= 3);

      for(auto& p : points)
        vertices.push_back(new Vertex{p});

      vertices[0]->prev = vertices[size-1];
      for (auto i = 0; i < size; ++i)
      {
        vertices[i]->next = vertices[(i+1)%size];
        if(i > 0)
          vertices[i]->prev = vertices[i-1];
      }
    }

    std::vector<Geom::Point2d> Polygon::GetEars()
    {
      std::vector<Geom::Point2d> ears;
      for(auto v : vertices) {
        if(v->is_ear)
          ears.push_back(v->point);    
      }
      return ears;
    }

  }

}