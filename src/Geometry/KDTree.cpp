#include "KDTree.h"
#include "GeomUtils.h"

namespace Geom
{
  KDTree2D::KDTree2D(const std::vector<Point2d>& points)
  {
    if(points.empty())
      return;
    m_root = BuildTree(0,points);  
  }

  KDTree2D::KDTree2D(std::vector<Point2d>&& points)
  {
    if(points.empty())
      return;
    m_root = BuildTree(0,std::move(points));
  }

  KDTree2D::KDNode2D* KDTree2D::BuildTree(uint32_t depth, std::vector<Point2d> points)
  {
    //Could store multiple points in a leaf node, so use vector
    uint32_t num_points = points.size();
    if(num_points == 1) {
      KDNode2D* node = new KDNode2D();
      node->is_leaf = true;
      node->depth = depth;
      node->points.push_back(points[0]);
      return node;
    }
 
    if(depth%2 == 0) //split on vertical axis => sort by x coord
      std::sort(points.begin(), points.end(), [](Point2d a, Point2d b) {return a.x < b.x;});
    else //split on horiz axis => sort by y coord
      std::sort(points.begin(), points.end(), [](Point2d a, Point2d b) {return a.y < b.y;});    

    uint32_t median_pos = (num_points%2 == 0) ? num_points/2 : num_points/2 + 1;
    float split_value = (depth%2 == 0) ? points[median_pos-1].x : points[median_pos-1].y;

    std::vector<Point2d> first_half(points.begin(), points.begin() + median_pos);
    std::vector<Point2d> second_half(points.begin() + median_pos, points.end());  

    KDNode2D* node = new KDNode2D();
    node->is_leaf = false;
    node->split_value = split_value;
    node->depth = depth;
    node->left = BuildTree(depth+1, std::move(first_half));
    node->right = BuildTree(depth+1, std::move(second_half));
    return node;
  }

  std::vector<Point2d> KDTree2D::BruteForceRangeSearch(const Range& input_range)
  {
    std::vector<Point2d> all_points;
    AccumulateSubtreePoints(m_root, all_points);
    std::vector<Point2d> points_in_range;
    for(auto& p : all_points) {
      if(RangeContainsPoint(p,input_range))
        points_in_range.push_back(p);
    }
    return points_in_range;
  }

  std::vector<Point2d> KDTree2D::RangeSearch(const Range& input_range)
  {
    std::vector<Point2d> points_found;
    Range node_range;
    SearchNode(m_root, node_range, input_range, points_found);
    return points_found;
  }

  std::vector<Point2d> KDTree2D::CollectAllPoints()
  {
    std::vector<Point2d> points;
    AccumulateSubtreePoints(m_root,points);
    return points;
  }

  void KDTree2D::ValidateSearch(const Range& input_range)
  {
    auto points1 = RangeSearch(input_range);
    auto points2 = BruteForceRangeSearch(input_range);

    SPG_ASSERT(points1.size() == points2.size());
    for(uint32_t i = 0; i<points1.size(); ++i) {
      auto p1 = points1[i];
      int matches = 0; 
      for(uint32_t j=0; j<points2.size(); ++j) {
        auto p2 = points2[j];
        if(Geom::Equal(p1,p2))
          matches++;
      }
      SPG_ASSERT(matches == 1);
    }
  }

  void KDTree2D::SearchNode(KDNode2D* node, Range node_range, const Range& input_range,std::vector<Point2d>& points_found)
  {
    SPG_ASSERT(node != nullptr);

    if(node->is_leaf) {
      SPG_ASSERT(node->points.size() == 1);
      auto p = node->points[0];
      if(RangeContainsPoint(p, input_range)) {
        points_found.push_back(p);
      }
      return;
    }

    //update node range
    Range r_left = node_range;
    Range r_right = node_range;
    if(node->depth % 2 == 0) { //vertical split
      SPG_ASSERT((node_range.x_max > node->split_value)&&(node_range.x_min < node->split_value));
      r_left.x_max = node->split_value;
      r_right.x_min = node->split_value;
    }
    else { //horizontal splt
      SPG_ASSERT((node_range.y_max > node->split_value)&&(node_range.y_min < node->split_value));
      r_left.y_max = node->split_value;
      r_right.y_min = node->split_value;
    }

    if(RangeContainsRange(input_range, r_left)) {
      AccumulateSubtreePoints(node->left, points_found);
    }
    else if(RangesIntersect(input_range, r_left)) {
      SearchNode(node->left, r_left, input_range, points_found);
    }

    if(RangeContainsRange(input_range, r_right)) {
      AccumulateSubtreePoints(node->right, points_found);
    }
    else if(RangesIntersect(input_range, r_right)) {
      SearchNode(node->right, r_right, input_range, points_found);
    }
  }

  void KDTree2D::AccumulateSubtreePoints(KDNode2D* node,std::vector<Point2d>& cur_points)
  {
    SPG_ASSERT(node != nullptr);
    if(node->is_leaf) {
      SPG_ASSERT(node->points.size() == 1);
      cur_points.push_back(node->points[0]);
      return;
    }
    AccumulateSubtreePoints(node->left,cur_points);
    AccumulateSubtreePoints(node->right,cur_points);
  }

  bool KDTree2D::RangeContainsPoint(Point2d p, const Range& range)
  {
    if(!(p.x < range.x_max))
      return false;
    if(p.x < range.x_min)
      return false;
    if(!(p.y < range.y_max))
      return false;
    if(p.y < range.y_min)
      return false;

    return true;   
  }

  bool KDTree2D::RangeContainsRange(const Range& range, const Range& test_range)
  {
    if(!(test_range.x_max < range.x_max))
      return false;
    if(test_range.x_min < range.x_min)
      return false;
    if(!(test_range.y_max < range.y_max))
      return false;
    if(test_range.y_min < range.y_min)
      return false;
    
    return true;   
  }

  bool KDTree2D::RangesIntersect(const Range& range1, const Range& range2)
  {
    bool no_intersection = (range1.x_min > range2.x_max) || (range1.x_max < range2.x_min) || 
      (range1.y_min > range2.y_max) || (range1.y_max < range2.y_min);

    return !no_intersection;
  }

}