#include "IntersectionSet.h"
#include "GeomUtils.h"
#include <Common/Common.h>
#include <string>
#include <algorithm>

namespace Geom
{
  namespace ItersectSet
  {
    void Event::Print() 
    {
      SPG_WARN("({},{})",  point.x, point.y);
      for(auto seg : seg_list) {
        SPG_TRACE("  ({},{}) -> ({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
      }
      SPG_INFO("----------------");
    }

    void Queue::Print() 
    {
      for(auto itr = m_data.cbegin(); itr != m_data.end(); itr++) {
        Event e = *itr;
        e.Print();
      }
    }
    
    void StatusStructure::Print()
    {
      SPG_WARN("Status structure----------------------------------");
      SPG_TRACE("y sweep: {}: ",m_y_sweep);
      for(auto& seg : m_T) {
        SPG_TRACE("  ({},{}) -> ({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
      }
      SPG_WARN("End Status ------------------------------");
    }

    void IntersectionSet::Print()
    {
      SPG_WARN("Intersections Found----------------------------------");
      for(auto& intersection : m_intersections) {
        SPG_TRACE("({},{})",  intersection.point.x, intersection.point.y);
        for(auto& seg : intersection.segs) {
          SPG_TRACE("  ({},{}) -> ({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
      }
      SPG_WARN("End ------------------------------");
    }

    void Queue::Insert(const SegList& seg_list)
    {
      for(const auto& seg : seg_list) {
        SPG_ASSERT(!Geom::Equal(seg.start,seg.end));
        auto upper = seg.start;
        auto lower = seg.end;
        if(!Geom::Equal(seg.start.y, seg.end.y)) {
          if((upper.y < lower.y)) {
            upper = seg.end;
            lower = seg.start;    
          }
        }
        else if(upper.x > lower.x) { //horizontal line.  Left point is considered the upper endpoint
          upper = seg.end;
          lower = seg.start;
        }
        LineSeg2D new_seg{upper,lower}; //Ensure start point is upper
        {
          auto [itr,success] = m_data.emplace(upper, new_seg); 
          if(!success)
          { //upper point already in queue - add this seg to the existing event point
            auto event = *itr;
            m_data.erase(itr); //need to erase, update, re-insert since elements in set are const
            event.Insert(new_seg);
            m_data.insert(event);  
          }
        }
        {
          auto [itr,success] = m_data.emplace(lower); 
          auto event = *itr;
          if(success) { //The lower should never be a duplicated, so should always be true
            if(itr != m_data.begin()) {
              auto prev_itr = std::prev(itr);
              //If y difference between consecutive events is < s_sweep_delta-y, then set y values equal
              if((*prev_itr).point.y - event.point.y < StatusStructure::s_sweep_delta) {
                event.point.y = (*prev_itr).point.y;
                m_data.erase(itr);
                m_data.insert(event);  
              }
            }
          }
        }
        
      }
    }

    void Queue::Insert(Event e)
    {
      m_data.insert(e); 
    }

    Event Queue::Next() 
    {
      auto itr = m_data.cbegin();
      Event e = *itr;
      m_data.erase(itr);
      return e;
    }

    const float StatusStructure::s_sweep_delta = 0.001f;
    
    void StatusStructure::Delete_LC()
    {
      for(auto seg : m_union_LC) {
        m_T.erase(seg);
      }
    }

    void StatusStructure::Insert_UC()
    {
      m_y_sweep -= s_sweep_delta; //first lower sweep line a tad to ensure segs in C are swapped
      m_sweep_delta_applied = true;
      for(auto seg : m_union_UC) {
        m_T.insert(seg);
      }
    }

    void StatusStructure::Update(const Event& e)
    {
      m_y_sweep = e.point.y;
      m_sweep_delta_applied = false;
      m_union_LUC.clear();
      m_union_LC.clear();
      m_union_UC.clear();

      SegList u, l, c; //upper, lower, central (i.e. interior) segs
      u = e.seg_list;
      for(auto& seg : m_T) {
        if(Geom::Equal(e.point, seg.end)) 
           l.insert(l.end(), seg);  
        else if(SegContainsPoint(seg, e.point)) 
          c.insert(c.end(), seg);
      }

      LineSeg2dComparator comp = LineSeg2dComparator();
      std::sort(l.begin(), l.end(),comp);
      std::sort(u.begin(), u.end(),comp);
      std::sort(c.begin(), c.end(),comp);

      std::set_union(l.cbegin(), l.cend(), c.cbegin(), c.cend(), std::back_inserter(m_union_LC),comp);
      std::set_union(u.cbegin(), u.cend(), c.cbegin(), c.cend(), std::back_inserter(m_union_UC),comp);
      std::sort(m_union_LC.begin(), m_union_LC.end(),comp);
      std::set_union(u.cbegin(), u.cend(), m_union_LC.cbegin(), m_union_LC.cend(), std::back_inserter(m_union_LUC),comp);
    }

    auto StatusStructure::LeftAndRightNeighbour(Point2d p)
    {
      auto itr = std::max_element(std::cbegin(m_T), std::cend(m_T), SweepLineComparator(m_y_sweep, p.x));
      //std::pair<decltype(itr), decltype(itr)> result;
      if(itr != m_T.end() && std::next(itr) != m_T.end()) 
        return std::make_pair(itr, std::next(itr));
      
      return std::make_pair(m_T.end(), m_T.end());
    }

    auto StatusStructure::LeftMost_UC_In_T(Point2d p)
    {
      auto uc_itr = std::min_element(m_union_UC.cbegin(), m_union_UC.cend(), SweepLineComparator(m_y_sweep));
      auto element = *uc_itr;
      auto itr = m_T.find(element);
      if(itr != m_T.end() && itr != m_T.begin()) 
        return std::make_pair(std::prev(itr),itr);
      
      return std::make_pair(m_T.end(), m_T.end());
    }

    auto StatusStructure::RightMost_UC_In_T(Point2d p)
    {
      auto uc_itr = std::max_element(m_union_UC.cbegin(), m_union_UC.cend(), SweepLineComparator(m_y_sweep));
      auto element = *uc_itr;
      auto itr = m_T.find(element);
      if(itr != m_T.end() && std::next(itr) != m_T.end()) 
        return std::make_pair(itr, std::next(itr));
        
      return std::make_pair(m_T.end(), m_T.end());
    }

    IntersectionSet::IntersectionSet(const SegList& seg_list)
    {
      m_queue.Insert(seg_list);
    }

    void IntersectionSet::Process()
    {
      while(!m_queue.IsEmpty()) {
        Event e = m_queue.Next();
        e.Print();
        HandleEvent(e);
        m_status.Print();
      }
      Print();
    }

    void IntersectionSet::FindNewEvent(const LineSeg2D& seg1, const LineSeg2D& seg2,Point2d p)
    {
      if(!StrictIntersectionExists(seg1, seg2)) 
        return;
  
      Point2d intersection_point;
      bool success = ComputeIntersection(seg1, seg2, intersection_point);
      if(success) { //check not really needed - if no intersection StrictIntersectionExists() returned false
        if(intersection_point.y < m_status.SweepLineY() || (intersection_point.x > p.x)) {
          Event e{intersection_point};
          m_queue.Insert(e); 
        }
      }
    }

    void IntersectionSet::HandleEvent(const Event& e)
    {
      m_status.Update(e);

      if(m_status.Get_LUC().size() >= 2)
         m_intersections.emplace_back(e.point, m_status.Get_LUC());

      m_status.Delete_LC();
      m_status.Insert_UC();

      if(m_status.Get_UC().empty()) {
        auto [left_itr, right_itr] = m_status.LeftAndRightNeighbour(e.point); 
        if(left_itr !=  m_status.end() && right_itr !=  m_status.end())
          FindNewEvent(*left_itr, *right_itr,e.point);    
      }
      else {
        {
          auto [left_itr, right_itr] = m_status.LeftMost_UC_In_T(e.point); 
          if(left_itr !=  m_status.end() && right_itr !=  m_status.end())
            FindNewEvent(*left_itr, *right_itr, e.point);   
        }
        {
          auto [left_itr, right_itr] = m_status.RightMost_UC_In_T(e.point); 
          if(left_itr !=  m_status.end() && right_itr !=  m_status.end())
            FindNewEvent(*left_itr, *right_itr, e.point);     
        }
      }
    }

  }
}