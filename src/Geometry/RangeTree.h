#pragma once
#include "RBTree.h"
#include "GeomUtils.h"
#include <Common/Common.h>

namespace Geom
{
  class RangeTree1D
  {
    using Tree1D = Geom::Set<float>;

  public:

    struct CompY
    {
      bool operator () (const Point2d& p1, const Point2d& p2)  {return p1.y < p2.y;}
    };
    
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
    void Search(Tree1D::RBNode* node, const Range& range, std::vector<float>& out);
    Tree1D m_tree;
  };


  class RangeTree2D
  {

    struct Comp
    { //only for validation 
      bool operator () (const Point2d& p1, const Point2d& p2) 
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
      bool operator () (const Point2d& p1, const Point2d& p2) {return (p1.x < p2.x);}
    };

    struct CompY
    {
      bool operator () (const Point2d& p1, const Point2d& p2)  {return p1.y < p2.y;}
    };

    using SecondaryTree = Geom::Set<Geom::Point2d,CompY>;
    using SecondaryNode = SecondaryTree::RBNode;

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
      void SeachSubTreeSecondary(SecondaryTree& tree, SecondaryNode* node, const Range& range,std::vector<Point2d>& points_out);

      void ReportSubTreeMain(Node* node, std::vector<Point2d>& out_points); //For validation
      void ValidateTree(Node* node); //for validation
      std::vector<Point2d> RangeQueryX(const Range& range); //for validation
      std::vector<Point2d> BruteForceRangeQuery(const Range& range); //For validation
  
    private:
      Node* m_root;
  };

}