#pragma once
#include "GeomUtils.h"
#include "Line.h"
#include <vector>
#include <Common/Common.h>
#include <set>
#include <map>
#include <variant>
#include <iostream>

 /*
  Comparators must have 'Strict week ordering'
  1. Irreflexivity: comp(a,a) == false  
  2. Asymmetry: if comp(a,b) == true then comp(b,a) == false
  3. Transivity: if comp(a, b) && comp(b, c) then comp(a, c) == true
  4. Transivity of equivalence: if !comp(a, b) && !comp(b, a)  and  !comp(b, c) && !comp(c, b)
    then !comp(a, c) && !comp(c, a)

   ChatGPT:
        For std::set:
        Since std::set requires unique elements based on the comparator, if comp(existing, val) == false and comp(val, existing) == false, then val is considered a duplicate and will not be inserted.

        Comp(v1,v2) == true, v1 goes before v2.  Comp(v1,v1) == true implies an invalid comparator
        Comp(v1,v2) == false v1 does not go before v2. 
        
        For a std::set<T, Compare>, the comparator is called as:
        comp(existing_element, val);
        If comp(existing, val) == true, then val is considered to go after existing, so it continues checking other elements.
        If no existing element is considered equivalent to val, it will be inserted.
        If comp(existing, val) == false and comp(val, existing) == false, then val is considered equivalent to existing, and std::set does not insert it.  
*/

namespace Geom
{
  namespace ItersectSet
  {
    /*
      Bentley-Ottmann - Line intersection algorithm. Comp Geom book, sec 2.1
    */
    
    using SegList = std::vector<LineSeg2D>;
    
    struct Event
    {
      Event() = default;
      Event(Point2d point) : point{point} {}
      Event(Point2d point, LineSeg2D seg) : point{point}  {
        seg_list.push_back(seg);
      }
      Event(Point2d point, const SegList& segs) : point{point}, seg_list(segs) {}

      void Insert(LineSeg2D seg) {
        seg_list.push_back(seg);
      }
      void Print();

      Point2d point;
      SegList seg_list;
    };

    struct EventComparator
    {
      bool operator ()(const Event& e1,  const Event& e2) const noexcept
      {
        if( !Geom::Equal(e1.point.y,e2.point.y))
          return e1.point.y > e2.point.y;  
        else
          return e1.point.x < e2.point.x;  
      }
    };

    class Queue
    {
      public:
        Queue() = default;
        ~Queue() = default;

        void Insert(const SegList& seg_list);
        void Insert(Event e);
        bool IsEmpty() const {
          return m_data.empty();
        }
        Event Next();
        void Print();

      private:  
        std::set<Event, EventComparator> m_data;
    };

    struct ComparatorLogger
    {
      struct ComparisonRecord
      {
        LineSeg2D seg1;
        LineSeg2D seg2;
        Point2d event_point;
        bool result;
        int tag;
      };
      struct PreInsertionRecord
      {
        LineSeg2D seg;
      };
       struct PostInsertionRecord
      {
        LineSeg2D seg;
        bool success;
        int32_t size_change=0;
      };
      struct PreDeletionRecord
      {
        LineSeg2D seg;
      };
      struct PostDeletionRecord
      {
        LineSeg2D seg;
        bool success;
        int32_t size_change=0;
      };

      using Record = std::variant<ComparisonRecord,PreInsertionRecord,PostInsertionRecord,PreDeletionRecord,PostDeletionRecord>;

      void Log(const LineSeg2D& seg1, const LineSeg2D& seg2, const Point2d& event_point, bool result, int tag) {
        ComparisonRecord record{seg1,seg2,event_point,result,tag};
        m_data.push_back(record);
      }

      void Log(Record record) {
        m_data.push_back(record);
      }
      void Print();

      std::vector<Record> m_data;
    };  

    struct SegComparator
    {
      bool operator ()(const LineSeg2D& seg1,  const LineSeg2D& seg2) const noexcept
      {
        if(!Equal(seg1.start.x,seg2.start.x))
          return seg1.start.x < seg2.start.x;
        else if(!Equal(seg1.start.y,seg2.start.y))  
          return seg1.start.y > seg2.start.y;
        else if(!Equal(seg1.end.x,seg2.end.x))
          return seg1.end.x < seg2.end.x;  
        else
          return seg1.end.y > seg2.end.y;     
      }
    };

    struct SegEqualityChecker
    {
      LineSeg2D target;
      explicit SegEqualityChecker(LineSeg2D& target) : target{target} {}
      bool operator ()(const LineSeg2D& seg) const
      {
        if(!Equal(seg.start.x,target.start.x))
          return false;
        else if(!Equal(seg.start.y,target.start.y))  
          return false;
        else if(!Equal(seg.end.x,target.end.x))
          return false;
        else if(!Equal(seg.end.y,target.end.y))
          return false;
        return true;  
      }
    };

    struct SweepLineComparator
    {
      Point2d& event_point;
      SweepLineComparator(Point2d& event_point_) : event_point{event_point_} {}
      SweepLineComparator(Point2d& event_point_, ComparatorLogger* logger) : 
        event_point{event_point_}, m_logger{logger} {}

      float ComputeSweepLineXIntercept(const LineSeg2D& seg) const noexcept;
      bool operator ()(const LineSeg2D& seg1,  const LineSeg2D& seg2) const noexcept;

      ComparatorLogger* m_logger = nullptr;
      void SetComparatorLogger(ComparatorLogger* logger) {m_logger = logger;}
    };

    class StatusStructure
    {
      public:

        StatusStructure() : m_T(SweepLineComparator(m_cur_event_point, &m_comparator_log)) {}

        void FindNewActiveSegCandidates(const Event& e);
        auto FindSegsInTContainingPoint(const Point2d& p);
      
        auto LeftAndRightNeighbour(const Event& e);
        auto LeftMost_UC_In_T(const Event& e);
        auto RightMost_UC_In_T(const Event& e);

        void UpdateActiveSegs(const Event& e);

        SegList& Get_LUC() {return m_union_LUC;}
        SegList& Get_LC() {return m_union_LC;}
        SegList& Get_UC() {return m_union_UC;}
        auto& GetStatusStructure() {return m_T;}
        auto begin() {return m_T.begin();}
        auto end() {return m_T.end();}
        auto rbegin() {return m_T.rbegin();}
        auto rend() {return m_T.rend();}

        float SweepLineY() const {
          return m_cur_event_point.y;
        }
        ComparatorLogger& GetComparatorLogger() {
          return m_comparator_log;
        }
        void PrintComparatorLog() {
          m_comparator_log.Print();
        }

        void PrintStatusStructure();
        void PrintUnionUC(const Point2d& p);
        void PrintStatusStructureSubset(std::set<LineSeg2D, SweepLineComparator>::iterator first,
         std::set<LineSeg2D, SweepLineComparator>::iterator last, const Point2d& p);

        void PrintActiveSegList(); 

       static const float s_sweep_delta; //Amount to lower sweep line to force order swap for intersecting segs  

      private:
        Point2d m_cur_event_point{FLT_MAX,FLT_MAX};
        std::set<LineSeg2D, SweepLineComparator> m_T; //ordered set of active segs. i.e. 'Status Structure'
        SegList m_union_LUC, m_union_LC, m_union_UC;
        ComparatorLogger m_comparator_log;
        SegList m_active_segs;
    };
    
    struct Intersection
    {
      Point2d point;
      SegList segs;
    };

    class IntersectionSet 
    {
      public:
        IntersectionSet() = default;
        IntersectionSet(const SegList& seg_list);
        void Process();  
        void PrintIntersections();
      private:
        void HandleEvent(const Event& e);
        void FindNewEvent(const LineSeg2D& seg1, const LineSeg2D& seg2, Point2d p);
      private:
        Queue m_queue;
        StatusStructure m_status;
        std::vector<Intersection> m_intersections;
    };

  }
}