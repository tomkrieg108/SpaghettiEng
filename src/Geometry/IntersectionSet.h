#pragma once
#include "GeomBase.h"
#include "Line.h"
#include <vector>
#include <set>
#include <map>

//https://copilot.microsoft.com/chats/4vj5Cni1Tz4j2oQt6sqLh

//https://copilot.microsoft.com/shares/qWy8WdJVqsSxLmmQek3uC

namespace Geom
{
  namespace ItersectSet
  {
    /*
      Bentley-Ottmann - Line intersection 
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
      //Todo.  This can be a lambda
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

    struct LineSeg2dComparator
    {
      //Need this, but the specific ordering shouldn't matter
      //Todo.  This can be a lambda
      //Todo - use std::tie() for this
      bool operator ()(const LineSeg2D& seg1,  const LineSeg2D& seg2) const noexcept
      { 
        if(!Geom::Equal(seg1.start.y, seg2.start.y))
          return seg1.start.y > seg2.start.y;
        else if (!Geom::Equal(seg1.start.x, seg2.start.x))  
          return seg1.start.x < seg2.start.x;
        if(!Geom::Equal(seg1.end.y, seg2.end.y))
          return seg1.end.y > seg2.end.y;  
        else
          return seg1.end.x < seg2.end.x;  
      }
    };

    struct SweepLineComparator
    {
      float& y_sweep;
      float x_limit = FLT_MAX;

      SweepLineComparator(float& y_sweep_) : y_sweep{y_sweep_} {}
      SweepLineComparator(float& y_sweep_, float x_limit_) : y_sweep{y_sweep_}, x_limit{x_limit_} {}

      float ComputeSweepLineXIntercept(const LineSeg2D& seg) const noexcept
      {
        float y = y_sweep;
        if(Geom::Equal(seg.start.y, y))
          return seg.start.x;
      
        if(Geom::Equal(seg.end.y, y))
          return seg.end.x;

        if(Geom::Equal(seg.start.x, seg.end.x))  
          return seg.start.x;

        if(Geom::Equal(seg.start.y, seg.end.y))  
          return seg.start.x;

        float x = (seg.start.x - seg.end.x) / (seg.start.y - seg.end.y) *(y - seg.end.y) + seg.end.x;
        return x;
      }

      bool operator ()(const LineSeg2D& seg1,  const LineSeg2D& seg2) const noexcept
      { 
        float x1 = ComputeSweepLineXIntercept(seg1);
        float x2 = ComputeSweepLineXIntercept(seg2);
        //Todo - what to do if the 2 segs yield the same x-intercept.
        //todo - revisit this second constraint w.r.t. 'strict weak ordering'
        return (x1 < x2) && (x2 < x_limit);
      }
    };

    class StatusStructure
    {
      public:

        StatusStructure() : m_T(SweepLineComparator(m_y_sweep)) {}

        void Update(const Event& e);
      
        auto LeftAndRightNeighbour(Point2d p);
        auto LeftMost_UC_In_T(Point2d p);
        auto RightMost_UC_In_T(Point2d p);

        void Delete_LC();
        void Insert_UC();

        SegList& Get_LUC() {return m_union_LUC;}
        SegList& Get_LC() {return m_union_LC;}
        SegList& Get_UC() {return m_union_UC;}
        auto& GetStatusStructure() {return m_T;}
        auto begin() {return m_T.begin();}
        auto end() {return m_T.end();}
        auto rbegin() {return m_T.rbegin();}
        auto rend() {return m_T.rend();}

        float SweepLineY() const {
          return m_sweep_delta_applied ? m_y_sweep + s_sweep_delta : m_y_sweep;
        }

        void Print();

        static const float s_sweep_delta; //Amount to lower sweep line to enforce order swap for intersecting segs

      private:
        float m_y_sweep = FLT_MAX;
        bool m_sweep_delta_applied = false;
        std::set<LineSeg2D, SweepLineComparator> m_T; //set for sweep line status structure
        SegList m_union_LUC, m_union_LC, m_union_UC;
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
        void Print();
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