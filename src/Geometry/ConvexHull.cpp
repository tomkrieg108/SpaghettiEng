
#include "Geometry/ConvexHull.h"

#include <vector>

#include "CoreLib/Core.h"

#include "Geometry/GeomBase.h"
#include "Geometry/Line.h"
#include "Geometry/MeshPrimitives2D.h"
#include "Geometry/GeomUtils.h"

namespace Geom
{
  std::vector<Geom::Point2d> ConvexHull2D_GiftWrap(const std::vector<Geom::Point2d>& points)
  {
    std::vector<Geom::Point2d> hull;
    if(points.size() < 3)
      return hull;

    //Get the start point (bottom most)
    float y_min = std::numeric_limits<float>::max();
    uint32_t start_idx = 0;
    for(auto i=0; i< points.size(); ++i)  
    {
      if(points[i].y < y_min)
      {
        y_min = points[i].y;
        start_idx = i;
      }
    }
    hull.push_back(points[start_idx]);
    
    //Get second point
    Geom::Point2d ref_point{points[start_idx].x +100.0f, points[start_idx].y}; //horiz line seg to the right
    Geom::LineSeg2D ref_seg{points[start_idx], ref_point};
    float min_angle = std::numeric_limits<float>::max();
    uint32_t second_idx = 0;
    for(auto i=0; i< points.size(); ++i)  
    {
      if(i == start_idx) 
        continue;

      Geom::LineSeg2D test_seg{points[start_idx], points[i]};
      float angle = Geom::ComputeAngleInDegrees(ref_seg,test_seg);
      if(angle < min_angle)
      {
        min_angle = angle;
        second_idx = i;  
      }
    }
    hull.push_back(points[second_idx]);

    uint32_t prev_idx = start_idx;
    uint32_t cur_idx = second_idx;
    while(true)
    {
      Geom::LineSeg2D ref_seg{points[prev_idx], points[cur_idx]};
      float min_angle = std::numeric_limits<float>::max();
      uint32_t next_idx = 0;
      for(auto i=0; i<points.size(); ++i)
      {
        Geom::LineSeg2D test_seg{points[cur_idx], points[i]};
        float angle = Geom::ComputeAngleInDegrees(ref_seg,test_seg);
        if(angle < min_angle)
        {
          min_angle = angle;
          next_idx = i;  
        }
      }
      if(next_idx != start_idx)
      {
        hull.push_back(points[next_idx]);
        prev_idx = cur_idx;
        cur_idx = next_idx;
      }
      else
        break;
    }

    return hull;
  }

  std::vector<Geom::Point2d> Convexhull2D_ModifiedGrahams(const std::vector<Geom::Point2d>& points)
  {
    if(points.size() < 3)
      return points;
      
    //sort points left to right
    std::vector<Geom::Point2d> sorted_points = points;
    std::sort(std::begin(sorted_points), std::end(sorted_points), [](const Geom::Point2d& lhs, const Geom::Point2d& rhs) {
        return lhs.x < rhs.x;
    });
    
    //generate upper hull
    std::vector<Geom::Point2d> upper_hull;
    for(auto itr = sorted_points.begin(); itr != sorted_points.end(); ++itr )
    {
      while( (upper_hull.size() > 1) && Geom::Left({*(upper_hull.cend()-2), *(upper_hull.cend()-1)}, *itr))
        upper_hull.pop_back();

      upper_hull.push_back(*itr);
    }

    //generate lower hull
    std::vector<Geom::Point2d> lower_hull;
    for(auto itr = sorted_points.rbegin(); itr != sorted_points.rend(); ++itr )
    {
      while( (lower_hull.size() > 1) && Geom::Left({*(lower_hull.cend()-2), *(lower_hull.cend()-1)}, *itr))
        lower_hull.pop_back();

      lower_hull.push_back(*itr);
    }

    //merge lower into upper
    upper_hull.insert(upper_hull.end(), lower_hull.begin()+1, lower_hull.end()-1);
    return upper_hull;
  }


}