#include "IntersectionSet.h"
#include "GeomUtils.h"
#include <Common/Common.h>
#include <string>
#include <algorithm>

namespace Geom
{
  namespace ItersectSet
  {
    #define LOG_COMP_VERS 0
    #define ENABLE_PRINTING
    //#define ENABLE_PRINT_COMPARATOR_LOGGING

    static void PrintComparatorResult1(const LineSeg2D& seg1, const LineSeg2D& seg2, const Point2d& event_point, bool result, int idx)
    {
      const char* res_str = result ? "True" : "False" ;
      SPG_INFO("  {}.SLC Comparing ({},{})->({},{}) with ({},{})->({},{}) at ({},{}) => {}",idx, seg1.start.x,seg1.start.y,seg1.end.x,seg1.end.y, seg2.start.x,seg2.start.y,seg2.end.x,seg2.end.y, event_point.x,event_point.y, res_str);
    }

    static void PrintComparatorResult2(const LineSeg2D& seg1, const LineSeg2D& seg2, const Point2d& event_point, bool result, int idx)
    {
      std::cout << idx << ".SLC Comp: " ;
      std::cout << "(" << seg1.start.x << "," << seg1.start.y << ")->(" << seg1.end.x << "," << seg2.end.y << ") with";
      std::cout << "(" << seg2.start.x << "," << seg2.start.y << ")->(" << seg2.end.x << "," << seg2.end.y << ")  at ";
      std::cout << "(" << event_point.x << "," << event_point.y << ") => " << std::boolalpha << result;
      std::cout << std::endl; 
    }

    #if(LOG_COMP_VERS == 1)
      #define PRINT_COMPARATOR_RESULT(seg1,seg2,event_point,result,idx) PrintComparatorResult1(seg1,seg2,event_point,result,idx)
    #elif(LOG_COMP_VERS == 2) 
       #define PRINT_COMPARATOR_RESULT(seg1,seg2,event_point,result,idx) PrintComparatorResult2(seg1,seg2,event_point,result,idx)
    #else
       #define PRINT_COMPARATOR_RESULT(seg1,seg2,event_point,result,tag)   
    #endif
   
    #define LOG_COMP_RES_THEN_RETURN(seg1,seg2,event_point,result,tag) \
      { \
        if(m_logger != nullptr) { \
          ComparatorLogger::ComparisonRecord record{seg1,seg2,event_point,result,tag}; \
          m_logger->Log(record); \
        } \
        return result; \
      } \
    
    static void PrintInsertion(LineSeg2D& seg)
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_WARN("Inserting ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
      SPG_LOG_FLUSH;
    }

    static void PrintDeletion(LineSeg2D& seg)
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_WARN("Deleting ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
      SPG_LOG_FLUSH;
    }

    static void PrintInsertionResult(LineSeg2D& seg, int diff)
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
        if(diff == 1) {
           SPG_INFO("Insertion Success ({},{})->({},{}), {}", seg.start.x,seg.start.y,seg.end.x,seg.end.y, diff);
        } else {
           SPG_ERROR("Insertion Fail ({},{})->({},{}), {}", seg.start.x,seg.start.y,seg.end.x,seg.end.y, diff);
        }
        SPG_LOG_FLUSH;
    }

    static void PrintDeletionResult(LineSeg2D& seg, int diff)
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
        if(diff == -1) {
           SPG_INFO("Deletion Success ({},{})->({},{}), {}", seg.start.x,seg.start.y,seg.end.x,seg.end.y, diff);
        } else {
           SPG_ERROR("Deletion Fail ({},{})->({},{}), {}", seg.start.x,seg.start.y,seg.end.x,seg.end.y, diff);
        }
        SPG_LOG_FLUSH;
    }

    void ComparatorLogger::Print()
    {
      #ifndef ENABLE_PRINT_COMPARATOR_LOGGING
        return;
      #endif
      for(auto& record : m_data) {
        if(std::holds_alternative<ComparatorLogger::ComparisonRecord>(record)){
          auto r = std::get<ComparatorLogger::ComparisonRecord>(record);
          const char* res_str = r.result ? "True" : "False" ;
          SPG_TRACE("{}.SLC COMPARING ({},{})->({},{}) with ({},{})->({},{}) at ({},{}) => {}",r.tag, r.seg1.start.x, r.seg1.start.y, r.seg1.end.x, r.seg1.end.y, r.seg2.start.x, r.seg2.start.y, r.seg2.end.x, r.seg2.end.y, r.event_point.x, r.event_point.y, res_str);
        }
        else if (std::holds_alternative<ComparatorLogger::PreInsertionRecord>(record)) {
          auto r = std::get<ComparatorLogger::PreInsertionRecord>(record);
          SPG_WARN("Inserting: ({},{})->({},{})", r.seg.start.x, r.seg.start.y, r.seg.end.x, r.seg.end.y );
        }
        else if (std::holds_alternative<ComparatorLogger::PreDeletionRecord>(record)) {
          auto r = std::get<ComparatorLogger::PreDeletionRecord>(record);
          SPG_WARN("Deleting: ({},{})->({},{})", r.seg.start.x, r.seg.start.y, r.seg.end.x, r.seg.end.y );
        }
        else if (std::holds_alternative<ComparatorLogger::PostInsertionRecord>(record)) {
          auto r = std::get<ComparatorLogger::PostInsertionRecord>(record);
          if(r.success) 
            SPG_INFO("Insertion Successful: ({},{})->({},{})", r.seg.start.x, r.seg.start.y, r.seg.end.x, r.seg.end.y)
          else
             SPG_ERROR("Insertion Failed: ({},{})->({},{}). Delta size: {}", r.seg.start.x, r.seg.start.y, r.seg.end.x, r.seg.end.y, r.size_change)
        }
        else if (std::holds_alternative<ComparatorLogger::PostDeletionRecord>(record)) {
          auto r = std::get<ComparatorLogger::PostDeletionRecord>(record);
          if(r.success) 
            SPG_INFO("Deletion Successful: ({},{})->({},{})", r.seg.start.x, r.seg.start.y, r.seg.end.x, r.seg.end.y)
          else
             SPG_ERROR("Deletion Failed: ({},{})->({},{}). Delta size: {}", r.seg.start.x, r.seg.start.y, r.seg.end.x, r.seg.end.y, r.size_change)
        }
      }
    }

    void Event::Print()
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_WARN("Next Event: ({},{}) ----------------",  point.x, point.y);
      for(auto seg : seg_list) {
        SPG_TRACE("  ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
      }
    }

    void Queue::Print()  
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      for(auto itr = m_data.cbegin(); itr != m_data.end(); itr++) {
        Event e = *itr;
        e.Print();
      }
    }
    
    void StatusStructure::PrintStatusStructure()
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_TRACE("Status structure: y sweep {}: ------------------- ",m_cur_event_point.y);
      for(auto& seg : m_T) {
        if(IsHorizontal(seg)) {
          SPG_TRACE("  ({},{})->({},{}) - HOR", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
        else {
          SPG_TRACE("  ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
      }
    }

    void StatusStructure::PrintStatusStructureSubset(std::set<LineSeg2D, SweepLineComparator>::iterator first,
         std::set<LineSeg2D, SweepLineComparator>::iterator last, const Point2d& p)
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_WARN("Status structure - Subset of segs containing ({},{}) ----------------------", p.x,p.y);
      SPG_ASSERT((first != m_T.end()) && (last != m_T.end()));
      for(auto itr = first; itr != std::next(last); ++itr) {
        LineSeg2D seg = *itr;
        if(IsHorizontal(seg)) {
          SPG_TRACE("  ({},{})->({},{}) - HOR", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
        else {
          SPG_TRACE("  ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
      }
    }

    void StatusStructure::PrintActiveSegList()
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_TRACE("Segs in m_active_segs !!! ------------------- ");
      for(auto& seg : m_active_segs) {
        if(IsHorizontal(seg)) {
          SPG_TRACE("  ({},{})->({},{}) - HOR", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
        else {
          SPG_TRACE("  ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
      }
    }

    void StatusStructure::PrintUnionUC(const Point2d& p)
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_WARN("U(P) Union C(P) for ({},{}) ----------------------------------------", p.x,p.y);
      for(auto& seg : m_union_UC) {
        if(IsHorizontal(seg)) {
          SPG_TRACE("  ({},{})->({},{}) - HOR", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
        else {
          SPG_TRACE("  ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
      }
    }

    void IntersectionSet::PrintIntersections()
    {
      #ifndef ENABLE_PRINTING
        return;
      #endif
      SPG_WARN("Intersections Found----------------------------------");
      for(auto& intersection : m_intersections) {
        SPG_TRACE("({},{})",  intersection.point.x, intersection.point.y);
        for(auto& seg : intersection.segs) {
          SPG_TRACE("  ({},{})->({},{})", seg.start.x,seg.start.y,seg.end.x,seg.end.y);
        }
      }
    }

/*****************************************************************************************************
    HERE'S THE START OF THE CODE THAT ACTUALLY DOES STUFF!!
 ***************************************************************************************************/

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
        LineSeg2D new_seg{upper,lower}; //Ensure start point is segments upper point

        // Insert an event for the upper endpoint.
        {
          auto [itr,success] = m_data.emplace(upper, new_seg); 
          if(!success)
          { //upper point already in queue - add this seg to the existing event point
            auto event = *itr;
            m_data.erase(itr); //erase, update, re-insert since std::set elements are immutable
            event.Insert(new_seg);
            m_data.insert(event);  
          }
        }
        // Insert an event for the lower endpoint.
        {
          auto [itr,success] = m_data.emplace(lower); 
          auto event = *itr;
          if(success) { // lower endpoint should normally be unique.
            if(itr != m_data.begin()) {
              auto prev_itr = std::prev(itr);
              // Check if the y difference between consecutive events is less than s_sweep_delta.
              if((*prev_itr).point.y - event.point.y < StatusStructure::s_sweep_delta) {
                // If the difference is too small, align the event’s y to keep ordering consistent.
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

    float SweepLineComparator::ComputeSweepLineXIntercept(const LineSeg2D& seg) const noexcept
    {
      float y_sweep = event_point.y;
      if(Geom::Equal(seg.start.y, y_sweep))
        return seg.start.x;
      if(Geom::Equal(seg.end.y, y_sweep))
        return seg.end.x;
      if(Geom::IsVertical(seg))
        return seg.start.x;  
      if(IsHorizontal(seg)) { 
        if(SegIncludesPoint(seg,event_point))
          return event_point.x;
        else  
          return seg.start.x;  
      }
      
      y_sweep = event_point.y - 0.01f;  
      
      float x = (seg.start.x - seg.end.x) / (seg.start.y - seg.end.y) *(y_sweep - seg.end.y) + seg.end.x;

      auto seg_min_x = std::min(seg.start.x,seg.end.x);
      auto seg_max_x = std::max(seg.start.x,seg.end.x);  
      SPG_ASSERT((x >= seg_min_x) && (x <= seg_max_x));

      return x;
    }

    bool SweepLineComparator::operator ()(const LineSeg2D& seg1,  const LineSeg2D& seg2) const noexcept
    {
      if(Geom::Equal(seg1.start, seg2.start) && Geom::Equal(seg1.end, seg2.end)) {
        LOG_COMP_RES_THEN_RETURN(seg1,seg2,event_point,false,1); //segs equivalent
      }
        
      bool s1_includes_p = Geom::SegIncludesPoint(seg1,event_point);
      bool s2_includes_p = Geom::SegIncludesPoint(seg2,event_point);
      bool s1_horiz = Geom::IsHorizontal(seg1);
      bool s2_horiz = Geom::IsHorizontal(seg2);
#if 1
      if(s1_includes_p && s2_includes_p) {
        if(s2_horiz && !s1_horiz) {
          LOG_COMP_RES_THEN_RETURN(seg1,seg2,event_point, true, 2); //seg1 (non horizontal) comes before seg2 (horizontal)
        }
        if(s1_horiz && !s2_horiz) {
          LOG_COMP_RES_THEN_RETURN(seg1,seg2,event_point, false,3); 
        }
        if(s1_horiz && s2_horiz) {
          bool res = std::tie(seg1.start.x, seg1.end.x) < std::tie(seg2.start.x, seg2.end.x);
          LOG_COMP_RES_THEN_RETURN(seg1,seg2,event_point,res,4);
        }
      }
#endif        

      //One or both of seg1, seg2 both don't coincide with the event point.  Or, both do coincide with event point but both are non-horizontal.

      float x1 = ComputeSweepLineXIntercept(seg1);
      float x2 = ComputeSweepLineXIntercept(seg2);
      
      //Todo - if sweep line is lowered slightly, then x-intercepts should not usually not be equal.  Just need to account for (near) vertical, horiz segs
      // if(!Equal(x1,x2)) 
      //   return x1 < x2; //if true, seg1 will come before seg 2

      if (std::fabs(x1 - x2) > Epsilon()) {
        LOG_COMP_RES_THEN_RETURN(seg1,seg2,event_point,x1<x2,5); //seg1 before seg2 if true
      }
        
      bool res = std::tie(seg1.start.x, seg1.start.y, seg1.end.x, seg1.end.y) <
          std::tie(seg2.start.x, seg2.start.y, seg2.end.x, seg2.end.y);

      LOG_COMP_RES_THEN_RETURN(seg1,seg2,event_point,res,6);
    }

    //Todo - consider a trailing return type as per Copilot info
    //The Segs containing point p in T (status struct) should be consecutive
    auto StatusStructure::FindSegsInTContainingPoint(const Point2d& p)
    {
      auto ret_val = std::make_pair(m_T.begin(),m_T.end());
      while( (ret_val.first != m_T.end()) && !Geom::SegIncludesPoint(*(ret_val.first),p) ) {
        ret_val.first++;
      }

      if(ret_val.first == m_T.end()) 
        return ret_val;
      
      ret_val.second = ret_val.first;
      while( (std::next(ret_val.second) != m_T.end()) ) {
        auto seg = *(std::next(ret_val.second));
        if(Geom::SegIncludesPoint(seg,p))
          ret_val.second++;
        else
          break; 
      }
      return ret_val;
    }

    const float StatusStructure::s_sweep_delta = 0.01f;

    void StatusStructure::UpdateActiveSegs(const Event& e)
    {
      m_T.clear();
      for(auto& seg : m_active_segs) {
         m_T.insert(seg);
      }
      SPG_ASSERT(m_T.size() == m_active_segs.size());
      return;

      for(auto seg : m_union_LC) {
        int size1 = m_T.size();
        ComparatorLogger::PreDeletionRecord deletion{seg};
        m_comparator_log.Log(deletion);
        PrintDeletion(seg);
        m_T.erase(seg);
        int size2 = m_T.size();
        PrintDeletionResult(seg, size2-size1);
        ComparatorLogger::PostDeletionRecord post_deletion{seg, (size2 - size1 == -1),  (size2 - size1)};
        m_comparator_log.Log(post_deletion);
      }
      //e.point.y -= s_sweep_delta;
      for(auto seg : m_union_UC) {
          int size1 = m_T.size();
          ComparatorLogger::PreInsertionRecord insertion{seg};
          m_comparator_log.Log(insertion);
          PrintInsertion(seg);
          m_T.insert(seg);
          int size2 = m_T.size();
          PrintInsertionResult(seg, size2-size1);
          ComparatorLogger::PostInsertionRecord post_insertion{seg, (size2 - size1 == 1),  (size2 - size1)};
          m_comparator_log.Log(post_insertion);
      }
      //e.point.y += s_sweep_delta;
    }

    void StatusStructure::FindNewActiveSegCandidates(const Event& e)
    {
      //SweepLineComparator holds refs to the next to variable updated according to event e
      this->m_cur_event_point = e.point; 

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

      //Using SweepLineComparator as sorting predicate with sweep line adjusted down.  Should give same order as in the status structure m_T after m_union_LC deleted and m_union_UC added!
      Point2d event_point = e.point;
      SweepLineComparator comp = SweepLineComparator(event_point);
      std::sort(l.begin(), l.end(),comp);
      std::sort(u.begin(), u.end(),comp);
      std::sort(c.begin(), c.end(),comp);

      std::set_union(l.cbegin(), l.cend(), c.cbegin(), c.cend(), std::back_inserter(m_union_LC),comp);
      std::set_union(u.cbegin(), u.cend(), c.cbegin(), c.cend(), std::back_inserter(m_union_UC),comp);
      std::sort(m_union_LC.begin(), m_union_LC.end(),comp);
      std::set_union(u.cbegin(), u.cend(), m_union_LC.cbegin(), m_union_LC.cend(), std::back_inserter(m_union_LUC),comp);

      //m_active_segs.insert(m_active_segs.end(), u.begin(), u.end());
      //m_active_segs.insert(m_active_segs.end(), c.begin(), c.end());
      for(auto& seg : m_union_LC) {
        auto itr = std::find_if(m_active_segs.begin(), m_active_segs.end(), SegEqualityChecker(seg));
        if(itr != m_active_segs.end())
          m_active_segs.erase(itr);
      }
      m_active_segs.insert(m_active_segs.end(), m_union_UC.begin(), m_union_UC.end());

      
#ifdef SPG_DEBUG
      //Pretty sure theres no way that l,c,u can every overlap - check..
      SPG_ASSERT(m_union_LUC.size() == l.size() + u.size() + c.size());
      SegList LC_i, UC_i, LUC_i;
      std::set_intersection(l.cbegin(), l.cend(), c.cbegin(), c.cend(), std::back_inserter(LC_i),comp);
      std::set_intersection(u.cbegin(), u.cend(), c.cbegin(), c.cend(), std::back_inserter(UC_i),comp);
       std::sort(LC_i.begin(), LC_i.end(),comp);
      std::set_intersection(u.cbegin(), u.cend(), LC_i.cbegin(), LC_i.cend(), std::back_inserter(LUC_i),comp);
      SPG_ASSERT(LUC_i.size() == 0);
#endif
    }

    auto StatusStructure::LeftAndRightNeighbour(const Event& e)
    {
      //step 9 in Comp Geom pg 26
      LineSeg2D dummy_seg{{e.point.x,  e.point.y},{e.point.x,  e.point.y + 0.1f}};
      //Returns iterator to first element in m_T that is  greater or equal to dummy_seg
      //use m_T.lower_bound(dummy_seg); if need to find the first that is strictly greater than dummy_seg
      auto itr = m_T.lower_bound(dummy_seg); 
      if( (itr != m_T.end()) && (itr != m_T.begin()) )
        return std::make_pair(std::prev(itr), itr); //left and right neighbours pf p
      return std::make_pair(m_T.end(), m_T.end());
    }

    auto StatusStructure::LeftMost_UC_In_T(const Event& e)
    {
      Point2d event_point = e.point;
      SweepLineComparator comp = SweepLineComparator(event_point);
      //get iterator to the left most element in m_union_UC
      auto uc_itr = std::min_element(m_union_UC.cbegin(), m_union_UC.cend(),comp);
      auto left_most_element = *uc_itr;
      auto itr = m_T.find(left_most_element);
      if(itr != m_T.end() && itr != m_T.begin()) 
        return std::make_pair(std::prev(itr),itr);
      
      return std::make_pair(m_T.end(), m_T.end());
    }

    auto StatusStructure::RightMost_UC_In_T(const Event& e)
    {
       Point2d event_point = e.point;
      SweepLineComparator comp = SweepLineComparator(event_point);
      //get iterator to the left most element in m_union_UC 
      auto uc_itr = std::max_element(m_union_UC.cbegin(), m_union_UC.cend(),comp);
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
        m_status.PrintStatusStructure();
        m_status.PrintActiveSegList();
        m_status.PrintUnionUC(e.point);
        auto [first_itr, second_itr] = m_status.FindSegsInTContainingPoint(e.point);
        if((first_itr != m_status.end()) && (second_itr != m_status.end()))
           m_status.PrintStatusStructureSubset(first_itr,second_itr,e.point);
        PrintIntersections();
      }
      m_status.PrintComparatorLog();
    }

    void IntersectionSet::FindNewEvent(const LineSeg2D& seg1, const LineSeg2D& seg2,Point2d p)
    {
      if(!StrictIntersectionExists(seg1, seg2)) 
        return;
  
      Point2d intersection_point;
      bool success = ComputeIntersection(seg1, seg2, intersection_point);
      if(success) { //StrictIntersectionExists should guarantee this, but check anyway.
        //x coord check is to avoid adding events that are to the left of an already processed point
        if(intersection_point.y < m_status.SweepLineY() || (intersection_point.x > p.x)) {
          Event e{intersection_point};
          m_queue.Insert(e); 
        }
      }
    }

    void IntersectionSet::HandleEvent(const Event& e)
    {
      m_status.FindNewActiveSegCandidates(e);

      if(m_status.Get_LUC().size() >= 2)
         m_intersections.emplace_back(e.point, m_status.Get_LUC());

      m_status.UpdateActiveSegs(e);

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