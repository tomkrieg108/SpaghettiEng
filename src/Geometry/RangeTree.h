#pragma once
#include "GeomBase.h"
#include <iterator>
#include <vector>

namespace Geom
{
  class RangeTree2d
  {
  private:
    struct RTNode 
    {
      RTNode* left = nullptr;
      RTNode* right = nullptr;
      RTNode* associate = nullptr;
      float value = FLT_MAX; //non-leaf - splitting value
      Point2d point; //leaf - point

      RTNode() {}
    };

    struct RTRange
    {

    };

    RTNode* m_root = nullptr;
    RTNode* ConstructAssociate(std::vector<Point2d>&, int start, int end);
    RTNode*  ConstructRangeTree2D(std::vector<Point2d>&, int start, int end);
    bool IsLeaf(RTNode* node);
    RTNode* SplitNode(float min, float max, RTNode* root);
    void InOrderTraverse(RTNode* node,std::vector<Point2d>& list);
  public:
    RangeTree2d(std::vector<Point2d>& points);
    void FindMain(const Point2d& min, const Point2d& max, std::vector<Point2d>& list);
    void FindAssociate(RTNode* root, float min, float max, std::vector<Point2d>& list);
  };
  
}