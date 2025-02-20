#include "RangeTree.h"
#include <algorithm>

namespace Geom
{
  RangeTree2d::RangeTree2d(std::vector<Point2d>& points)
  {
    std::sort(points.begin(), points.end(), [](Point2d a, Point2d b) {return a.x < b.x;});
    m_root = ConstructRangeTree2D(points,0, points.size()-1);
  }

  RangeTree2d::RTNode*  RangeTree2d::ConstructRangeTree2D(std::vector<Point2d>& points, int start, int end)
  {
    auto start_itr = points.begin();
    auto end_itr = points.begin();  
    std::advance(start_itr,start);
    std::advance(end_itr,end);

    std::vector<Point2d> points_y;
    std::copy(start_itr, ++end_itr, std::back_inserter(points_y));
    std::sort(points_y.begin(), points_y.end(), [](Point2d a, Point2d b) {return a.y < b.y;});

    int distance = end - start;
    int mid = distance/2 + start;

    RTNode* root = new RTNode();
    root->value = points[mid].x;
    root->associate = ConstructAssociate(points_y,0, points_y.size()-1);

    if(distance == 0) { //at leaf
      root->point = points[mid];
    }
    else {
      root->left = ConstructRangeTree2D(points,start,mid);
      root->right = ConstructRangeTree2D(points,mid+1,end);
    }
    return root;
  }

  RangeTree2d::RTNode* RangeTree2d::ConstructAssociate(std::vector<Point2d>& points, int start, int end)
  {//stored in each non-leaf node of main tree
    int distance = end - start;
    int mid = distance/2 + start;

    RTNode* root = new RTNode();
    root->value = points[mid].y;

    if(distance == 0) { //at leaf
      root->point = points[mid];
    }
    else {
      root->left = ConstructAssociate(points,start,mid);
      root->right = ConstructAssociate(points,mid+1,end);
    }
     return root;
  }

   RangeTree2d::RTNode* RangeTree2d::SplitNode(float min, float max, RangeTree2d::RTNode* root)
   {
    auto v = m_root;
    if(root)
      v = root;

     while(!IsLeaf(v) && (max <= v->value || min > v->value) ) {
      if(max <= v->value)
        v = v->left;
      else  
        v = v->right;  
     }
     return v;
   }

   bool RangeTree2d::IsLeaf(RangeTree2d::RTNode* node)
   {
    return (node->left == nullptr) && (node->right == nullptr);
   }

  void RangeTree2d::InOrderTraverse(RangeTree2d::RTNode* node,std::vector<Point2d>& list)
  {
     if (!node)
			return;
		InOrderTraverse(node->left, list);
		list.push_back(node->point);
		InOrderTraverse(node->right, list);
  }
  
  void RangeTree2d::FindAssociate(RangeTree2d::RTNode* root, float min, float max, std::vector<Point2d>& list)
  { 
    if(!root)
      return;

    auto v_split = SplitNode(min,max,root);
    if(IsLeaf(v_split)) {
      if(v_split->point.y >= min && v_split->point.y < max)
        list.push_back(v_split->point);
    }
    else {
      //follow path to left boundary
      auto v = v_split->left;
      while(!IsLeaf(v)) {
        if(min <= v->value) {
          InOrderTraverse(v->right, list);
          v = v->left;
        }
        else
          v = v->right;
      }
      if(v->point.y >= min)
        list.push_back(v->point);

      //Follow path to right boundary
      v = v_split->right;
      while(!IsLeaf(v)) {
      if(min <= v->value) {
        InOrderTraverse(v->left, list);
        v = v->right;
      }
      else
        v = v->left;
      }
      if(v->point.y <= max)
        list.push_back(v->point);
    }
  }
 
  void RangeTree2d::FindMain(const Point2d& min, const Point2d& max, std::vector<Point2d>& list)
  {
    auto v_split = SplitNode(min.x,max.x,m_root);
    if(IsLeaf(v_split)) {
      if(v_split->value >= min.x && v_split->value < max.x)
        list.push_back(v_split->point);
    }
    else {
      //follow path to left boundary
      auto v = v_split->left;
      while(!IsLeaf(v)) {
        if(min.x <= v->value) {
          FindAssociate(v->right ? v->right->associate : nullptr, min.y, max.y, list);
          v = v->left;
        }
        else
          v = v->right;
      }
      if(v->point.x >= min.x)
        list.push_back(v->point);

      //Follow path to right boundary
      v = v_split->right;
      while(!IsLeaf(v)) {
      if(max.x >= v->value) {
        FindAssociate(v->left ? v->left->associate : nullptr, min.y, max.y, list);
        v = v->right;
      }
      else
        v = v->left;
      }
      if(v->point.x <= max.x)
        list.push_back(v->point);
    }
  }
  
}