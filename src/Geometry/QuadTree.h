#pragma once

#include "GeomBase.h"
#include "BoundingVolume.h"
#include <vector>

namespace Geom
{
  //Todo - update this
#define NUM_POINTS 4
#define INE 0
#define INW 1
#define ISE 2
#define ISW 3

  struct QDTNode 
  {
		QDTNode* NE = nullptr, * NW = nullptr, * SE = nullptr, * SW = nullptr;
		QDTNode* parent = nullptr;
		AABB box;
		Point2d point = {FLT_MAX, FLT_MAX};
		bool is_leaf = false;
	};

  class QuadTree
  {
  public:
    QuadTree() {}
		QuadTree(std::vector<Point2d> points);
		QuadTree(std::vector<Point2d> points, AABB& bounds); //bounds is the BB of the root node
		void BalanceTheTree();
		void GenerateMesh();

  private:
    QDTNode* m_root = nullptr;

  };
}