#pragma once
#include "RBTree.h"
#include "RBTreeTraversable.h"
#include "GeomUtils.h"
#include <Common/Common.h>

namespace Geom
{
  


  class RangeTree1D
  {
  #ifdef RBTREE_BASE_TRAVERSABLE
    using Tree1D = Geom::RBTree<float, void>;
  #else
   using Tree1D = Geom::RBTreeTraversable<float,void>;
  #endif
    using Iterator = typename Tree1D::Iterator;

  public:

    struct Range
    {
      float x_min = std::numeric_limits<float>::lowest(); 
      float x_max = std::numeric_limits<float>::max(); 
    };

    RangeTree1D() = default;
    RangeTree1D(const std::vector<float>& points) :m_tree(points) {}
    RangeTree1D(std::vector<float>&& points) noexcept :m_tree(std::move(points)) {}
    std::vector<float> RangeSearch(const Range& range);
    auto begin() {return m_tree.begin();}
    auto end() {return m_tree.end();}
    static void Test();

  private:
    void Search(Iterator itr, const Range& range, std::vector<float>& out);
  private:
    Tree1D m_tree;
  };


  class RangeTree2D
  {
    //only used int Test() for validation
    struct Comp
    { 
      bool operator () (const Point2d& p1, const Point2d& p2) const
      {
        if(p1.x < p2.x) 
          return true;
        if(p2.x < p1.x)
          return false;
        //x-values equal (equivalent)
        if(p1.y < p2.y) 
          return true;
        return false;
      }
    };

    struct CompX
    {
      bool operator () (const Point2d& p1, const Point2d& p2) const {return (p1.x < p2.x);}
    };

    struct CompY
    {
      bool operator () (const Point2d& p1, const Point2d& p2) const {return p1.y < p2.y;}
    };

  #ifdef RBTREE_BASE_TRAVERSABLE
    using SecondaryTree = Geom::RBTree<Geom::Point2d,void,CompY>;
  #else
   using SecondaryTree = Geom::RBTreeTraversable<Geom::Point2d,void,CompY>;
  #endif

    using Iterator = typename SecondaryTree::Iterator;

    struct Node
    {
      SecondaryTree secondary_tree;
      float x_val = 0;
      Node* left = nullptr;
      Node* right = nullptr;
      bool is_leaf = false;
      Point2d point; 
    };

    public:

      struct Range
      {
        float x_min = std::numeric_limits<float>::lowest(); 
        float x_max = std::numeric_limits<float>::max(); 
        float y_min = std::numeric_limits<float>::lowest();
        float y_max = std::numeric_limits<float>::max();
      };

      RangeTree2D() = default;
      RangeTree2D( std::vector<Geom::Point2d>& points);
      RangeTree2D(std::vector<Geom::Point2d>&& points) noexcept;
      std::vector<Point2d> RangeQuery(const Range& range);
     
      static void Test();

    private:
      Node* BuildTree(std::vector<Point2d> points);
      Node* FindSplitNode(float x_low, float x_high);
      bool PointInRange(Point2d, const Range& range);
      std::vector<Point2d> RangeQueryY(SecondaryTree& tree, const Range& range);
      void SeachSubTreeSecondary(SecondaryTree& tree, Iterator itr, const Range& range,std::vector<Point2d>& points_out);

      // The following for validation only
      void ReportSubTreeMain(Node* node, std::vector<Point2d>& out_points); 
      std::vector<Point2d> BruteForceRangeQuery(const Range& range);
      std::vector<Point2d> RangeQueryX(const Range& range); 
      void ValidateTree(Node* node);
    
    private:
      Node* m_root;
  };

}