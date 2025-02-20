#include "BST.h"

namespace Geom
{
  //Sec5 - Trees
  
  //Optional index param - indicates root elements value
  BST::BST(std::vector<float> values, uint32_t index )
  {
    m_root = new BSTNode(values[index]);
    for(auto i = 0; i<values.size(); ++i) {
      if(i != index)
        Insert(values[i]);
    }
  }

   BST::~BST()
   {
      Delete(m_root);
      m_root = nullptr;
   }

  void BST::Insert(float value)
  {
    if (!m_root) {
			m_root = new BSTNode(value);
			return;
		}
		
    BSTNode* temp = m_root;
    while (true) {
      if (value < temp->value) {
        if (temp->left)
          temp = temp->left;
        else {
          temp->left = new BSTNode(value);
          temp->left->parent = temp;
          break;
        }
      }
      else {
        if (temp->right)
          temp = temp->right;
        else {
          temp->right = new BSTNode(value);
          temp->right->parent = temp;
          break;
        }
      }
    }
  }

  bool BST::Delete(float value)
  {
    auto cur_node = Find(m_root,value);
    if(cur_node == nullptr)
      return false;
    
    auto cur_left = cur_node->left;
    auto cur_right = cur_node->right;
    if(IsLeaf(cur_node)) {
      Transplant(cur_node, nullptr);
    }
    else if(cur_left == nullptr) {
      Transplant(cur_node, cur_right);
    }
    else if (cur_right == nullptr) {
      Transplant(cur_node, cur_left);
    }
    else {
      auto right_min = MinValue(cur_right);
      if(right_min->parent != cur_node) {
        Transplant(right_min, right_min->right);
        right_min->right = cur_node->right;
        right_min->right->parent = right_min;
      }

      Transplant(cur_node,right_min);
      right_min->left = cur_node->left;
      right_min->left->parent = right_min;
    }
    delete cur_node;
    return true;
  }

  void BST::Delete(BSTNode* node)
  {
    if (!node)
			return;
    if(node->left != nullptr)
      Delete(node->left);
    else if(node->right != nullptr)
      Delete(node->right);    
		else
      delete node;
  }

  BST::BSTNode* BST::Find(BSTNode* node, float value)
  {
    auto current = node;
    while(current && current->value != value) {
      if(value < current->value)
        current = current->left;
      else
        current = current->right;  
    }
    return current;
  }

   BST::BSTNode* BST::Successor(BSTNode* node)
   {
   if(node == nullptr)
      return nullptr;
    
    if(node->right != nullptr) {
      return MinValue(node->right);
    }  

    auto cur_node = node;
    while(cur_node->parent->left != cur_node) {
      cur_node = cur_node->parent;
      if(cur_node == m_root)
        return nullptr;
    }
    return cur_node->parent;
   }

  BST::BSTNode* BST::Predecessor(BSTNode* node)
  {
    if(node == nullptr)
      return nullptr;
    
    if(node->left != nullptr) {
      return MaxValue(node->right);
    }  

    if(node->parent == nullptr) //node is the root node
      return nullptr;

    auto cur_node = node;
    while(cur_node->parent->right != cur_node) {
      cur_node = cur_node->parent;
      if(cur_node == m_root)
        return nullptr;
    }
    return cur_node->parent;
  }

  BST::BSTNode* BST::MinValue(BSTNode* node)
  { //Follow the left branch until a leaf is reached - this is the min
    if(m_root == nullptr)
      return nullptr;
    if(node == nullptr)
      node = m_root;

    auto temp = node;
    while(temp->left != nullptr)
      return(MinValue(temp->left));
      
    return temp;   
  }

 BST::BSTNode* BST::MaxValue(BSTNode* node)
  { //Follow the right branch until a leaf is reached - this is the min
    if(m_root == nullptr)
      return nullptr;
    if(node == nullptr)
      node = m_root;

    auto temp = node;
    while(temp->right != nullptr)
      return(MinValue(temp->right));
      
    return temp;   
  }

  bool BST::Successor(float value, float& successor)
  {
    auto node = Find(m_root,value);
    if(node == nullptr)
      return false;

    auto successor_node =  Successor(node);
    if(successor_node != nullptr) {
      successor = successor_node->value;
      return true;
    }
    return false;
  }

  bool BST::Predecessor(float value, float& predecessor)
  {
     auto node = Find(m_root,value);
    if(node == nullptr)
      return false;

    auto predexessor_node =  Predecessor(node);
    if(predexessor_node != nullptr) {
      predecessor = predexessor_node->value;
      return true;
    }
    return false;
  }

  void BST::InOrderTraverse(BSTNode* node,std::list<float>& list)
  {
    if (!node)
			return;
		InOrderTraverse(node->left, list);
		list.push_back(node->value);
		InOrderTraverse(node->right, list);
  }

  void BST::PreOrderTraverse(BSTNode* node, std::list<float>& list)
  {
    if (!node)
			return;
		list.push_back(node->value);
		InOrderTraverse(node->left, list);
		InOrderTraverse(node->right, list);
  }

  void BST::PostOrderTraverse(BSTNode* node, std::list<float>& list)
  {
    if (!node)
			return;
		InOrderTraverse(node->left, list);
		InOrderTraverse(node->right, list);
		list.push_back(node->value);
  }

  BST::BSTNode* BST::GetSplitNode(float min_val, float max_val)
  {
    auto v = m_root;
    while(!IsLeaf(v) && (max_val <= v->value || min_val > v->value)) {
      if(max_val <= v->value)
        v=v->left;
      else
        v = v->right;  
    }
    return v;
  }

  void BST::RangeQuery(float min_val, float max_val, std::list<float>& list)
  {
    auto v_split = GetSplitNode(min_val, max_val);
		if (IsLeaf(v_split)) {
			if (v_split->value >= min_val && v_split->value < max_val)
        list.push_back(v_split->value);
		}
		else {
			//Follow the path to left boundary
			auto v = v_split->left;
			while (!IsLeaf(v)) {
				if (min_val <= v->value) {
					InOrderTraverse(v->right, list);
          list.push_back(v->value);
          v = v->left;
				}
				else
					v = v->right;
			}
      if(v->value >= min_val)
        list.push_back(v->value);

			//Follow the path to left boundary
			v = v_split->right;
			while (!IsLeaf(v)) {
				if (max_val >= v->value) {
					InOrderTraverse(v->left, list);
          list.push_back(v->value);
          v = v->right;
				}
				else
					v = v->left;
			}
      if(v->value <= max_val)
        list.push_back(v->value);
		}
  }


  bool BST::IsLeaf(BSTNode* node)
  {
    if(node == nullptr)
      return false;
    
    bool l_null = node->left == nullptr;
    bool r_null = node->right == nullptr;

    return l_null && r_null;

    //return ( (node->left == nullptr) && (node->right == nullptr));
  }

  //u is replaced by v
  void BST::Transplant(BSTNode* u, BSTNode* v)
  {
    if (!u->parent)
			m_root = v;
		else if (u == u->parent->left)
			u->parent->left = v;
		else
			u->parent->right = v;

		if (v != nullptr)
			v->parent = u->parent;
  }


}