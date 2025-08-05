#pragma once
#include "GeomUtils.h"
#include <Common/Common.h>

namespace Geom
{

  class KDTree2D
  {
  private:

    struct KDNode2D
    {
      bool is_leaf = false;
      float split_value = 0;
      uint32_t depth=0;
      KDNode2D* left = nullptr;
      KDNode2D* right = nullptr;
      //Only storing 1 point in a leaf for now - could maybe store multiple so use vector
      std::vector<Point2d> points; 
    };

  public:

    struct Range
    {
      float x_min = std::numeric_limits<float>::lowest(); 
      float x_max = std::numeric_limits<float>::max(); 
      float y_min = std::numeric_limits<float>::lowest();
      float y_max = std::numeric_limits<float>::max();
    };

    KDTree2D(std::vector<Point2d>&& points);
    KDTree2D(const std::vector<Point2d>& points);
    std::vector<Point2d> RangeSearch(const Range& input_range);
    std::vector<Point2d> BruteForceRangeSearch(const Range& input_range); //For testing
    std::vector<Point2d> CollectAllPoints();
    void ValidateSearch(const Range& input_range);

    /*
      Additional:

      Nearest neighbour search (collision detection, spacial search)
      Point Insertion
      Point Deletion
      Balanced KD-construction - depth limit, use AABBs to better partition sparse regions, multiple points per leaf
      Serialization / Export
      Visualization/debug helpers - walk and print tree.  Export to .dot file (for Graphviz), show bounding boxes and splits
      Batch construction from file
      Thread safe parallel search
      3D version
      Use in conjunction with other spacial structures - BVH, quadtrees etc
    */

  private:
    KDNode2D* BuildTree(uint32_t depth, std::vector<Point2d> points);
    void AccumulateSubtreePoints(KDNode2D* node,std::vector<Point2d>& cur_points);
    void SearchNode(KDNode2D* node, Range node_range, const Range& input_range, std::vector<Point2d>& points_found);
    bool RangeContainsPoint(Point2d, const Range& range);
    bool RangeContainsRange(const Range& range, const Range& test_range);  //Is test_range fully contained in range?
    bool RangesIntersect(const Range& range1, const Range& range2);

  private:
    KDNode2D* m_root = nullptr;
  };


}