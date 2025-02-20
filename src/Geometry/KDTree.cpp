#include "KDTree.h"

namespace Geom
{
  KDTree::KDNode* KDTree::ConstructKDTree(std::list<Point2d>& data, uint32_t depth)
  {
    auto size = data.size();
    if(size == 1)
      return new KDNode(data.front());

    if(depth % 2 == 0) {
      data.sort([](Point2d a, Point2d b){return a.x < b.x;});
    }  
    else {
      data.sort([](Point2d a, Point2d b){return a.y < b.y;});
    }  

    auto mid = size / 2;
    auto mid_ptr = data.begin();

    std::advance(mid_ptr,mid);
    auto left_list = std::list<Point2d>(data.begin(), mid_ptr);
    auto right_list = std::list<Point2d>(mid_ptr, data.end());

    auto left_child = ConstructKDTree(left_list, depth+1);
    auto right_child = ConstructKDTree(right_list, depth+1);

    return new KDNode((*mid_ptr)[depth%2], left_child, right_child);
  }

  bool KDTree::IsLeaf(KDNode* node)
  {
    if(node == nullptr)
      return false;

    return ( (node->left == nullptr) && (node->right == nullptr));
  }

  void KDTree::Traverse(KDNode* node, std::list<Point2d> list)
  { //in order traverse
    if(node == nullptr)
      return;
    Traverse(node->left, list);
    if(IsLeaf(node))
      list.push_back(node->data);
    Traverse(node->right,list) ;
  }

  void KDTree::PreprocessBoundaries(KDNode* node, bool is_even_depth)
  {
    if (!node || IsLeaf(node))
		return;

    if (is_even_depth) {
      if (node->left) {
        node->left->boundary = node->boundary;
        node->left->boundary.right = node->val;
        PreprocessBoundaries(node->left, !is_even_depth);
      }

      if (node->right) {
        node->right->boundary = node->boundary;
        node->right->boundary.left = node->val;
        PreprocessBoundaries(node->right, !is_even_depth);
      }
    }
    else {
      if (node->left) {
        node->left->boundary = node->boundary;
        node->left->boundary.top = node->val;
        PreprocessBoundaries(node->left, !is_even_depth);
      }

      if (node->right) {
        node->right->boundary = node->boundary;
        node->right->boundary.bot = node->val;
        PreprocessBoundaries(node->right, !is_even_depth);
      }
    }
  }

  bool KDTree::IsInside(const KDTree::KDRegion& r1, const KDTree::KDRegion& r2)
  {
    if (r1.left >= r2.left && r1.right <= r2.right && r1.bot >= r2.bot && r1.top <= r2.top)
		  return true;
	  return false;
  }

  bool KDTree::IsInRange(Point2d p, const KDTree::KDRegion& r)
  {
    if (p.x >= r.left && p.x <= r.right && p.y >= r.bot && p.y <= r.top)
		  return true;
	  return false;
  }

  bool KDTree::RegionsIntersect(const KDTree::KDRegion& r1, const KDTree::KDRegion& r2)
  {
    if (r1.right < r2.left || r1.left > r2.right)
		  return false;
	  if (r1.top < r2.bot || r1.bot > r2.bot)
		  return false;
	  return true;
  }

  void KDTree::SearchKdTree(KDTree::KDNode* node, KDTree::KDRegion range, std::list<Point2d> list)
  {
    if (IsLeaf(node)) {
      if (IsInRange(node->data, range))
        list.push_back(node->data);
      }
    else {
      if (RegionsIntersect(node->left->boundary, range)) {
        Traverse(node->left, list);
      }
      else if (RegionsIntersect(node->left->boundary, range)) {
        SearchKdTree(node->left, range, list);
      }

      if (IsInside(node->right->boundary, range)) {
        Traverse(node->right, list);
      }
      else if (RegionsIntersect(node->right->boundary, range)) {
        SearchKdTree(node->right, range, list);
      }
    }
  }

  static float sqrd_distance(const Point2d& v1, const Point2d& v2) {
	return (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y);
}

  void KDTree::NearestNeighbour(KDTree::KDNode* node, const Point2d& value, float& current_distance, 
    bool even_depth, Point2d& current_nn)
  {
    if (IsLeaf(node)) {
		  auto distance = sqrd_distance(value, node->data);
      if (distance < current_distance) {
        current_distance = distance;
        current_nn = node->data;
      }
    }
    else {
      auto val_coord = even_depth? value.x : value.y;
      if (val_coord < node->val) {
        NearestNeighbour(node->left, value, current_distance, !even_depth, current_nn);
        if (fabs(val_coord - node->val) < current_distance)
          NearestNeighbour(node->right, value, current_distance, !even_depth, current_nn);
      }
      else {
        NearestNeighbour(node->right, value, current_distance, !even_depth, current_nn);
        if (fabs(val_coord - node->val) < current_distance)
          NearestNeighbour(node->left, value, current_distance, !even_depth, current_nn);
      }
    }
  }

}
