#include "QuadTree.h"

namespace Geom
{

  static void Partition(QDTNode* _parent, std::vector<Point2d> _points) {
	if (_points.size() == 0) {
		//_parent->isAEmptyNode = true;
		_parent->is_leaf = true;
		return;
	}
	else if (_points.size() == 1) {
		_parent->point = _points[0];
		_parent->is_leaf = true;
		return;
	}
	else {
		// Calcualte the boundaries of each child using the bounds of the parent
		auto box = _parent->box;
		float x_mid = (box.x_min + box.x_max) / 2;
		float y_mid = (box.y_min + box.y_max) / 2;

		AABB boxNW{ box.x_min, x_mid, y_mid, box.y_max };
		AABB boxNE{ x_mid, box.x_max, y_mid, box.y_max };
		AABB boxSW{ box.x_min, x_mid, box.y_min, y_mid };
		AABB boxSE{ x_mid, box.x_max, box.y_min, y_mid };

		// Paritition the _points to four list.
		std::vector<Point2d> pointsNW, pointsNE, pointsSW, pointsSE;
		for (auto& point : _points) {
			if (boxNW.IsInside(point))
				pointsNW.push_back(point);
			else if (boxNE.IsInside(point))
				pointsNE.push_back(point);
			else if (boxSW.IsInside(point))
				pointsSW.push_back(point);
			else
				pointsSE.push_back(point);
		}

		// Create 4 childs and set the bounds
		auto nodeNW = new QDTNode();
		auto nodeNE = new QDTNode();
		auto nodeSW = new QDTNode();
		auto nodeSE = new QDTNode();

		nodeNW->box = boxNW;
		nodeNE->box = boxNE;
		nodeSW->box = boxSW;
		nodeSE->box = boxSE;

		// Do the recursive call.
		Partition(nodeNW, pointsNW);
		Partition(nodeNE, pointsNE);
		Partition(nodeSW, pointsSW);
		Partition(nodeSE, pointsSE);

		_parent->NW = nodeNW;
		_parent->NE = nodeNE;
		_parent->SW = nodeSW;
		_parent->SE = nodeSE;
		nodeNW->parent = _parent;
		nodeNE->parent = _parent;
		nodeSW->parent = _parent;
		nodeSE->parent = _parent;
	}
}

static QDTNode* northNeighbor(QDTNode* _node, QDTNode* _root) {
	if (_node == _root)
		return nullptr;

	if (_node == _node->parent->SW)
		return _node->parent->NW;

	if (_node == _node->parent->SE)
		return _node->parent->NE;

	auto u = northNeighbor(_node->parent, _root);
	//if (!u || u->is_leaf || u->isAEmptyNode)
	if (!u || u->is_leaf)
		return u;
	else if (_node == _node->parent->NW)
		return u->SW;
	else
		return u->SE;
}

static QDTNode* southNeighbor(QDTNode* _node, QDTNode* _root) {
	if (_node == _root)
		return nullptr;

	if (_node == _node->parent->NW)
		return _node->parent->SW;

	if (_node == _node->parent->NE)
		return _node->parent->SE;

	auto u = southNeighbor(_node->parent, _root);
	//if (!u || u->is_leaf || u->isAEmptyNode)
	if (!u || u->is_leaf)
		return u;
	else if (_node == _node->parent->SW)
		return u->NW;
	else
		return u->NE;
}

static QDTNode* eastNeighbor(QDTNode* _node, QDTNode* _root) {
	if (_node == _root)
		return nullptr;

	if (_node == _node->parent->NW)
		return _node->parent->NE;

	if (_node == _node->parent->SW)
		return _node->parent->SE;

	auto u = eastNeighbor(_node->parent, _root);
	//if (!u || u->is_leaf || u->isAEmptyNode)
	if (!u || u->is_leaf)
		return u;
	else if (_node == _node->parent->NE)
		return u->NW;
	else
		return u->SW;
}

static QDTNode* westNeighbor(QDTNode* _node, QDTNode* _root) {
	if (_node == _root)
		return nullptr;

	if (_node == _node->parent->NE)
		return _node->parent->NW;

	if (_node == _node->parent->SE)
		return _node->parent->SW;

	auto u = westNeighbor(_node->parent, _root);
	//if (!u || u->is_leaf || u->isAEmptyNode)
	if (!u || u->is_leaf)
		return u;
	else if (_node == _node->parent->NW)
		return u->NE;
	else
		return u->SE;
}

  QuadTree:: QuadTree(std::vector<Point2d> points, AABB& bounds)
  {
    if (points.size() == 0)
		  return;
    else {
      m_root = new QDTNode();
      m_root->box = bounds;
      Partition(m_root, points);
	  }
  }

  static bool needToSplit(QDTNode* _node, QDTNode* root)
  {
	if (!_node)
		return false;
	
	auto nNbor = northNeighbor(_node, root);
	auto sNbor = southNeighbor(_node, root);
	auto wNbor = westNeighbor(_node, root);
	auto eNbor = eastNeighbor(_node, root);

	// Check if this leaf has to split
	bool hasToSplit = false;
	if (nNbor && !(nNbor->is_leaf)
		&& (!(nNbor->SW->is_leaf) || !( nNbor->SE->is_leaf))) {
		hasToSplit = true;
	}
	else if (sNbor && !(sNbor->is_leaf)
		&& (!(sNbor->NW->is_leaf) || !(sNbor->NE->is_leaf))) {
		hasToSplit = true;
	}
	else if (wNbor && !(wNbor->is_leaf )
		&& (!(wNbor->NE->is_leaf) || !(wNbor->SE->is_leaf))) {
		hasToSplit = true;
	}
	else if (eNbor && !(eNbor->is_leaf)
		&& (!(eNbor->NW->is_leaf ) || !(eNbor->SW->is_leaf))) {
		hasToSplit = true;
	}
	return hasToSplit;
}

static void getLeafNodes(QDTNode* _node, std::vector<QDTNode*>& _leafs) 
{
	if (!_node)
		return;
	//if (_node->is_leaf || _node->isAEmptyNode)
	if (_node->is_leaf)
		_leafs.push_back(_node);
	else {
		getLeafNodes(_node->NW, _leafs);
		getLeafNodes(_node->NE, _leafs);
		getLeafNodes(_node->SW, _leafs);
		getLeafNodes(_node->SE, _leafs);
	}
}

static bool isEndNode(QDTNode* _node) 
{
	//if (_node && (_node->is_leaf || _node->isAEmptyNode))
	if (_node && (_node->is_leaf))
		return true;
	return false;
}

  QuadTree::QuadTree(std::vector<Point2d> points)
  {

  }

  void QuadTree::BalanceTheTree()
  {
    std::vector<QDTNode*> leafNodes;
    getLeafNodes(m_root, leafNodes);

    while (!leafNodes.empty()) {
      auto leaf = leafNodes.back();
      leafNodes.pop_back();

      bool hasToSplit = needToSplit(leaf, m_root);
      if (hasToSplit) {
        // Split the this node in to four childs.
        auto box = leaf->box;
        float x_mid = (box.x_min + box.x_max) / 2;
        float y_mid = (box.y_min + box.y_max) / 2;

        AABB boxNW{ box.x_min, x_mid, y_mid, box.y_max };
        AABB boxNE{ x_mid, box.x_max, y_mid, box.y_max };
        AABB boxSW{ box.x_min, x_mid, box.y_min, y_mid };
        AABB boxSE{ x_mid, box.x_max, box.y_min, y_mid };

        // Create 4 childs and set the bounds
        auto nodeNW = new QDTNode();
        auto nodeNE = new QDTNode();
        auto nodeSW = new QDTNode();
        auto nodeSE = new QDTNode();

        nodeNW->box = boxNW;
        nodeNE->box = boxNE;
        nodeSW->box = boxSW;
        nodeSE->box = boxSE;

        nodeNW->is_leaf = true;
        nodeNE->is_leaf = true;
        nodeSW->is_leaf = true;
        nodeSE->is_leaf = true;
        
        if (boxNW.IsInside(leaf->point))
          nodeNW->point = leaf->point;
        else if (boxNE.IsInside(leaf->point))
          nodeNE->point = leaf->point;
        else if (boxSW.IsInside(leaf->point))
          nodeSW->point = leaf->point;
        else
          nodeSE->point = leaf->point;

        leaf->NW = nodeNW;
        leaf->NE = nodeNE;
        leaf->SW = nodeSW;
        leaf->SE = nodeSE;
        nodeNW->parent = leaf;
        nodeNE->parent = leaf;
        nodeSW->parent = leaf;
        nodeSE->parent = leaf;

        //Leaf is no longer a empty node or a leaf node
        leaf->is_leaf = false;

        leafNodes.push_back(nodeNW);
        leafNodes.push_back(nodeNE);
        leafNodes.push_back(nodeSW);
        leafNodes.push_back(nodeSE);

        // Check if neighbours have to split or not
        auto nNbor = northNeighbor(leaf, m_root);
        auto sNbor = southNeighbor(leaf, m_root);
        auto wNbor = westNeighbor(leaf, m_root);
        auto eNbor = eastNeighbor(leaf, m_root);

        if( isEndNode(nNbor) && needToSplit(nNbor,m_root))
          leafNodes.push_back(nNbor);

        if (isEndNode(sNbor) && needToSplit(sNbor, m_root))
          leafNodes.push_back(sNbor);

        if (isEndNode(eNbor) && needToSplit(eNbor, m_root))
          leafNodes.push_back(eNbor);

        if (isEndNode(wNbor) && needToSplit(wNbor, m_root))
          leafNodes.push_back(wNbor);
      }
    }
  }

  void QuadTree::GenerateMesh()
  {
    
  }
}