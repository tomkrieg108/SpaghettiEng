#include "BSTree.h"
#include "GeomUtils.h"

namespace Geom
{
   /************************************************************ 
    PUBLIC FUNCTIONS
  */
 
  BSTree::BSTree(const std::vector<float>& values)
  {
    for(auto& val : values)
      Insert(val);
  }

  BSTree::~BSTree()
  {
  }

  bool BSTree::Insert(float value)
  {
    if(m_root == nullptr) {
      m_root = new BSTNode(value);
      m_node_count++;
      return true;
    }

    BSTNode* cur = m_root;   
    while(true) {

      if(Geom::EqualRel(value, cur->value)) {
        return false;
      }

      if( value < cur->value ) {
        if(cur->left == nullptr) {
          cur->left = new BSTNode(value);
          m_node_count++;
          cur->left->parent = cur;
          return true;
        } else {
          cur = cur->left;
        }
      }
      else {
        if(cur->right == nullptr) {
          cur->right = new BSTNode(value);
          m_node_count++;
          cur->right->parent = cur;
          return true;
        } else {
          cur = cur->right;
        }
      }

    }
  }

  bool BSTree::Contains(float value)
  {
    return ( BSTree::Find(m_root, value) != nullptr);
  }

  float BSTree::Max()
  {
    BSTNode* node = BSTree::Max(m_root);

    if(node == nullptr)
      return FLT_MAX;

    return node->value;   
  }

  float BSTree::Min()
  {
    BSTNode* node = BSTree::Min(m_root);

    if(node == nullptr)
      return FLT_MIN;

    return node->value;   
  }

  uint32_t BSTree::Size()
  {
    return m_node_count;
  }

  float BSTree::Next(float value)
  {
    BSTNode* node = Find(m_root, value);
    auto next = Next(node);

    if(next != nullptr)
      return next->value;
    else
      return FLT_MAX;
  }  

  float BSTree::Previous(float value)
  {
    BSTNode* node = Find(m_root, value);
    auto prev = Previous(node);

    if(prev != nullptr)
      return prev->value;
    else
      return FLT_MIN;
  } 

  std::vector<float> BSTree::PreOrderTraverse()
  {
    std::vector<float> output;
    PreOrderTraverse(m_root, output);
    return output;
  }

  std::vector<float> BSTree::InOrderTraverse()
  {
    std::vector<float> output;
    InOrderTraverse(m_root, output);
    return output;   
  }

	std::vector<float> BSTree::PostOrderTraverse()
  {
    std::vector<float> output;
    PostOrderTraverse(m_root, output);
    return output;
  }

  void BSTree::Erase(float value)
  {
    BSTNode* node = Find(m_root, value);
    Erase(node);
  }

  /************************************************************ 
    PRIVATE FUNCTIONS
  */

  
  void BSTree::InOrderTraverse(BSTNode* node, std::vector<float>& values_out)
  {
    if(node == nullptr)
      return;

    InOrderTraverse(node->left, values_out);
    values_out.push_back(node->value);
    InOrderTraverse(node->right,values_out);
  }

  void BSTree::PreOrderTraverse(BSTNode* node, std::vector<float>& values_out)
  {
    if(node == nullptr)
      return;

    values_out.push_back(node->value);
    PreOrderTraverse(node->left, values_out);
    PreOrderTraverse(node->right,values_out);
  }

  void BSTree::PostOrderTraverse(BSTNode* node, std::vector<float>& values_out)
  {
    if(node == nullptr)
      return;

    PostOrderTraverse(node->left, values_out);
    PostOrderTraverse(node->right,values_out);  
    values_out.push_back(node->value);
  }


  bool BSTree::IsLeftChild(BSTNode* node)
  {
    if(node == nullptr)
      return false;
    if(node->parent == nullptr)
      return false;
    return node->parent->left == node;    
  }

  bool BSTree::IsRightChild(BSTNode* node)
  {
    if(node == nullptr)
      return false;
    if(node->parent == nullptr)
      return false;
    return node->parent->right == node;    
  }


  BSTree::BSTNode* BSTree::Find(BSTNode* node, float value)
  {
    while( (node != nullptr) && !Geom::EqualRel(value, node->value))  {
      if(value < node->value) 
        node = node->left;
      else if (value > node->value) 
        node = node->right;  
    }
    return (node);
  }

  BSTree::BSTNode* BSTree::Min(BSTNode* node)
  {
    if(node == nullptr)
      return nullptr;

    while( (node->left != nullptr)  )   
      node = node->left;

    return node;  
  }


  BSTree::BSTNode* BSTree::Max(BSTNode* node)
  {
    if(node == nullptr)
      return nullptr;

    while( (node->right != nullptr)  )   
      node = node->right;

    return node;  
  }

  //Based on in-order traversal
  //Min value in right tree
  //If no right tree, first ancestor node that has a left child
  BSTree::BSTNode* BSTree::Next(BSTNode* node)
  { 
    if(node == nullptr)
      return nullptr;

    if( node->right != nullptr)
      return Min(node->right);
    
    while(node->parent != nullptr) {
      if(IsLeftChild(node))
        return node->parent;
      else 
        node = node->parent;  
    }
    return nullptr;  
  }

  //Based on in-order traversal
  //max value in left tree
  //If no left  tree, first ancestor node that has a right   child
  BSTree::BSTNode* BSTree::Previous(BSTNode* node)
  {
    if(node == nullptr)
      return nullptr;

    if( node->left != nullptr)
      return Max(node->left);
  
    while(node->parent != nullptr) {
      if(IsRightChild(node))
        return node->parent;
      else 
        node = node->parent;  
    }
    
    return nullptr;  
  }

  void BSTree::Erase(BSTNode* node)
  {
    if(node == nullptr)
      return;

    if( (node->left == nullptr) && (node->right == nullptr)) {
      if(IsLeftChild(node))
        node->parent->left = nullptr;
      else if(IsRightChild(node))
        node->parent->right = nullptr;
      else //root node
        m_root = nullptr;

      delete node;  
      m_node_count--;
    }
  
    else if((node->left != nullptr) && (node->right == nullptr)) {
      node->left->parent = node->parent;
      if(IsLeftChild(node))
        node->parent->left = node->left ;
      else if(IsRightChild(node))
        node->parent->right = node->left;
      else
        m_root = node->left;  

      delete node;
      m_node_count--;
    }
  
    else if( (node->left == nullptr) && (node->right != nullptr) ) {
      node->right->parent = node->parent;
      if(IsLeftChild(node))
        node->parent->left = node->right;
      else if(IsRightChild(node))
        node->parent->right = node->right;
      else
        m_root = node->right;  

      delete node;
      m_node_count--;
    }
  
    else {  //node to delete has both left and right branch
      //Replace the node to erase with it's successor
      BSTNode* successor = Next(node);
      node->value = successor->value;
      Erase(successor);
    }
  }

  void BSTree::Test()
  {
    std::vector<float> vals{26,32,43,11,15,100,17,7,87,42,150,111, 27, 54,1,33,200,88,99,0};
    BSTree tree(vals);
    auto ordered_vals = tree.InOrderTraverse();
    SPG_WARN("Elements in BSTree {}", tree.Size())
    for(auto& v : ordered_vals) {
      SPG_TRACE(v);
    }
  }

}