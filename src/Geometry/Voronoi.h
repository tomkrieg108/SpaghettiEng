#pragma once
#include "Geometry/RBTree.h"
#include "Geometry/DCEL.h"
#include "Geometry/GeomUtils.h"
#include <queue>
#include <variant>
#include <tuple> //for std::tie

namespace Geom
{
 
#define VORONOI_VERS 1

#if defined(VORONOI_VERS) && ((VORONOI_VERS==1))
  inline namespace voronoi_v1 {}
#endif

inline namespace voronoi_v1
{
  class Voronoi
  {
  public:

    struct Node;
  
    struct SiteEvent
    {
      Point2d site;
      Point2d GetPoint() const {return site;}
    };

    struct CircleEvent
    {
      CircleEvent() = default;
      CircleEvent(const Point2d& p) : circle_bottom{p} {}
      Point2d circle_bottom;
      Node* leaf_going = nullptr; //leaf associated with the arc that will disappear on the event
      Point2d GetPoint() const {return circle_bottom;}
    };

    using Event = std::variant<SiteEvent, CircleEvent>;

    struct EventLess {
    bool operator()(const Event& a, const Event& b) const {
        return std::visit([](const auto& lhs, const auto& rhs) {
            // greater_tan puts smallest element at top
            if(lhs.GetPoint().y > rhs.GetPoint().y)
              return true;
            if(rhs.GetPoint().y > lhs.GetPoint().y)  
              return false;
            return lhs.GetPoint().x > rhs.GetPoint().x;

             //same as: above
            // (tie take lvalues, so need to do this)
            // auto y_left = lhs.GetPoint().y;
            // auto y_right = rhs.GetPoint().y;
            // auto x_left = lhs.GetPoint().x;
            // auto x_right = lhs.GetPoint().x;

            // return std::tie(y_left, x_left) >
            //       std::tie(y_right, x_right);
        }, a, b);
      }
    };

   
    //chatGPT: store a pointers to sites (points)
    struct Node
    {
      enum class Type {Internal, Leaf} type;

      //For Internal nodes (breakpoints)
      Point2d* left_site = nullptr;
      Point2d* right_site = nullptr;
      DCEL::HalfEdge* half_edge = nullptr;  //ptr to half edge being traced out by this breakpoint

      //For Leaf nodes (arcs)
      Point2d* site = nullptr;
      CircleEvent* circle_event = nullptr;
    }; 

    Voronoi() = default;
    Voronoi(std::vector<Geom::Point2d>& points);
    //Voronoi(std::vector<Geom::Point2d>&& points) noexcept {}
      
    static void Test();

  private:

    void BuildFiagram(std::vector<Geom::Point2d>& points);
    void HandleSiteEvent(SiteEvent e);
    void HandleCircleEvent(CircleEvent e);

    // For testing / validtion
    void AddEventsToQueue(){}
    void PrintQueu(){}

  private:
    Geom::DCEL m_diagram;
    Geom::Map<float,Node> m_status;
    std::priority_queue<Event, std::vector<Event>, EventLess> m_event_queue;
  };
}

//NOTE:
/*
  std:less (roughly):

  template<class T = void>
  struct less;

  specializtion for T = void:

  template<>
  struct less<void> {

      Tells STL that the comparator can compare different but comparable types
      using is_transparent = void; 

      template<class A, class B>
      constexpr bool operator()(A&& a, B&& b) const
          noexcept(noexcept(std::forward<A>(a) < std::forward<B>(b)))
      {
          return std::forward<A>(a) < std::forward<B>(b);
      }
  };

  Notice how the operator is a template — it can deduce A and B automatically.

  It means you can use std::less<> (or std::greater<>, etc.) without ever mentioning your custom type explicitly.
*/
 
namespace voronoi_v2 
{

}





#if 0
    using Site = Geom::Point2d;

    using SiteEvent = Site;
    

    struct Arc
    {
      //For Leaf nodes
      Site* site = nullptr;
      Event* circle_event = nullptr;
    };

    struct BreakPoint 
    {
      //For Internal nodes
      Site* left_site = nullptr;  //site for left arc
      Site* right_site = nullptr; //site for right arc
      DCEL::HalfEdge* half_edge = nullptr;  //ptr to half edge being traced out by this breakpoint
    };

    using Node = std::variant<Arc,BreakPoint>;

    struct CircleEvent
    {
      Geom::Point2d circle_low;
      Node* leaf_going = nullptr; //leaf associated with the acr that will disappear on the event
    };
    using Event = std::variant<SiteEvent, CircleEvent>;

    struct NodeData
    {
      enum class Type {Internal, Leaf} type;

      //For Internal nodes (breakpoints)
      Site* left_site = nullptr;
      Site* right_site = nullptr;

      //For Leaf nodes
      Site* site = nullptr;
      void* circle_event = nullptr;
    }; 
    
   
    class Voronoi
    {
    public:

      static void Test() {}

    private:
      //std::set<NodeData> status;
      //Geom::Map<float, NodeData> status;
      //std::map<float,Node> s2;

      Geom::DCEL m_diagram;
      Geom::Map<float,Node> m_status;
      std::priority_queue<Event, std::vector<Event>, std::greater<float>> m_event_queue; //order top->bottom
      //std::priority_queue<Event> m_event_queue;
      
    };
#endif

  
   


}