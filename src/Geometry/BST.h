#pragma once
#include <vector>
#include <list>

/*
  Process a set of data points to determine which points lie within a given range (1D,2D,3D)
  Orthogonal searches - boundaries are at right angles (i.e. a rectangular shape)
  Non-orthogonal 'simplex range searching' - range query could be polygonal - returning the value lying within the polygon.abort
  Use tree as data structure to perormd the query

  -Find splt node
  -follow search path of left boundary value
    Report all leaves in the right subtree
  -follow search path of right boundary value
    Report all leaves in the left subtree  



*/

namespace Geom
{
   //Sec5 - Trees

  class BST
  {
   
  private:
    struct BSTNode {
			//KType key = {};
			//VType value = {};
			BSTNode* left = nullptr;
			BSTNode* right = nullptr;
			BSTNode* parent = nullptr;
      float value;

		public:
			BSTNode() {}

			BSTNode(float _value, BSTNode* _left = nullptr, BSTNode* _right = nullptr, BSTNode* _parent = nullptr) :
				value(_value), left(_left), right(_right), parent(_parent) {}
		};


  public:

    BST() {}
    BST(std::vector<float> values, uint32_t index = 0 );
    ~BST();

    void Insert(float value);
    bool Delete(float value);

    //Todo - not ideal to return bool indicating success and op param as value
    bool Find(float value) {
      return (Find(m_root, value) != nullptr);
    }

    //Return node with min / max value
    BSTNode* MinValue(BSTNode* node = nullptr);
    BSTNode* MaxValue(BSTNode* node = nullptr);

    //Second arg is outout.  
    //Todo - not ideal to return bool indicating success and op param as value
    bool Successor(float value, float& successor);
    bool Predecessor(float value, float& predecessor);

    //Second arg is outout
    void InOrderTraverse(BSTNode* node,std::list<float>& list);
		void PreOrderTraverse(BSTNode* node, std::list<float>& list);
		void PostOrderTraverse(BSTNode* node, std::list<float>& list);

    BSTNode* GetSplitNode(float min_val, float max_val);
    void RangeQuery(float min_val, float max_val, std::list<float>& list);

    bool IsLeaf(BSTNode* node);
    BSTNode* GetRootNode() {
      return m_root;
    }

  private:
    BSTNode* m_root = nullptr;

  private:
    BSTNode* Find(BSTNode* node, float value);   
    BSTNode* Successor(BSTNode* node);  
    BSTNode* Predecessor(BSTNode* node);  
    void Transplant(BSTNode* u,BSTNode* v); //u is replaced by v
    void Delete(BSTNode* node);

  };
}