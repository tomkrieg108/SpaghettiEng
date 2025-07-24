#pragma once
#include <Common/Common.h>

namespace Geom
{
  //Todo - stack based recursion

  class BSTree
  {
    struct BSTNode
    {
      float value = 0;
      BSTNode* left = nullptr;
      BSTNode* right = nullptr;
      BSTNode* parent = nullptr;
      BSTNode() = default;
      BSTNode(float val) : value{val} {}
    };

  public:
    BSTree() = default;
    BSTree(const std::vector<float>& values);
    ~BSTree();

    bool Insert(float value);
    void Erase(float value);
    void Clear();
    bool Contains(float value);
    float Max();
    float Min();
    uint32_t Size();
    float Next(float value);   
    float Previous(float value);  

    std::vector<float> PreOrderTraverse();
    std::vector<float> InOrderTraverse();
		std::vector<float> PostOrderTraverse();

  private:
    void PreOrderTraverse(BSTNode* node, std::vector<float>& values_out);
    void InOrderTraverse(BSTNode* node, std::vector<float>& values_out);
    void PostOrderTraverse(BSTNode* node, std::vector<float>& values_out);
    bool IsLeftChild(BSTNode* node);
    bool IsRightChild(BSTNode* node);
    BSTNode* Find(BSTNode* node, float value);  
    BSTNode* Min(BSTNode* node);
    BSTNode* Max(BSTNode* node);
    BSTNode* Next(BSTNode* node);  
    BSTNode* Previous(BSTNode* node);  
    BSTNode* LowerBound(float value);  
    BSTNode* UpperBound(float value);  
    void Transplant(BSTNode* u,BSTNode* v); //u is replaced by v - not used or implemented
    void Erase(BSTNode* node);
    
  private:

    BSTNode* m_root = nullptr;
    uint32_t m_node_count = 0;

    
    
    std::vector<BSTNode*> m_node_array; //not used

  };


}