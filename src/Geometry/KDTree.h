#pragma once

#include "GeomBase.h"
#include <list>

namespace Geom
{
    class KDTree 
    {

      struct KDRegion 
      {
        float left,right, bot, top;
      };

      struct KDNode
      {
        KDNode* left;
        KDNode* right;
        Point2d data;
        float val = FLT_MIN;
        KDRegion boundary{-10,10,-10,10};

        KDNode(float value, KDNode* left = nullptr, KDNode* right = nullptr) : val{value} {}
        KDNode(Point2d data, KDNode* left = nullptr, KDNode* right = nullptr) : data{data} {}

      };

    KDNode* m_root = nullptr;

    KDRegion default_bound{-10,10,-10,10};
    KDRegion invalid{0,0,0,0};

    KDNode* ConstructKDTree(std::list<Point2d>& data, uint32_t depth);
    void PreprocessBoundaries(KDNode* node, bool is_even_depth = true);
    void SearchKdTree(KDNode* node, KDRegion range, std::list<Point2d> list);
    void NearestNeighbour(KDNode* node, const Point2d& value, float& current_distance, bool even_depth, Point2d& current_nn);

  public:
    KDTree() {};
    KDTree(std::list<Point2d>& data){
      m_root = ConstructKDTree(data,0);
      m_root->boundary = default_bound;
      PreprocessBoundaries(m_root);
    }
    bool IsLeaf(KDNode* node);
    void Traverse(KDNode* node, std::list<Point2d> list);
    //Return true if r1 is completely contained within r2
    bool IsInside(const KDRegion& r1, const KDRegion& r2);
    bool IsInRange(Point2d p, const KDRegion& r);
    bool RegionsIntersect(const KDRegion& r1, const KDRegion& r2);

  };

}