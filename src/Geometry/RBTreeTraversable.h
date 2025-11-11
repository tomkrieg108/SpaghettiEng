#pragma once
#include "RBTree.h"

namespace Geom
{
#ifndef RBTREE_BASE_TRAVERSABLE
  template<
    typename TKey,
    typename TVal,
    typename TComp = std::less<TKey>,
    typename Traits = DefaultTraits<TKey,TVal>
  >
  class RBTreeTraversable : public RBTree<TKey,TVal,TComp,Traits>
  {
    using Base = RBTree<TKey,TVal,TComp,Traits>;
    using RBNode = typename Base::RBNode;
    using key_type =  typename Base::key_type;
    using value_type = typename Base::value_type;
  
  public:
    using Iterator = typename Base::Iterator;

    Iterator Root() {
      return Iterator(Base::m_root,Base::m_nil);
    }
    Iterator Parent(Iterator itr) const {
      return Iterator(itr.m_node->parent,Base::m_nil);
    }
    Iterator LeftChild(Iterator itr) {
      return Iterator(itr.m_node->left,Base::m_nil);
    }
    Iterator RightChild(Iterator itr) {
      return Iterator(itr.m_node->right,Base::m_nil);
    }
    bool IsLeaf(Iterator itr) {
      return Base::IsLeaf(itr.m_node);
    }
    bool IsRoot(Iterator itr) {
      return itr.m_node == Base::m_root;
    }
    bool HasLeft(Iterator itr) {
      return itr.m_node->left != Base::m_nil;
    }
    bool HasRight(Iterator itr) {
      return itr.m_node->right != Base::m_nil;
    }
    Iterator FindSplitPos(const key_type& key_low, const key_type& key_high)
    {
      SPG_ASSERT(Base::Less(key_low,key_high));
      RBNode* node = Base::m_root;
      while( !Base::IsLeaf(node) && (!Base::Less(node->value,key_high) || Base::Less(node->value,key_low)) ) {
        if(!Base::Less(node->value,key_high))
          node = node->left;
        else
          node = node->right;  
      }
      return Iterator(node,Base::m_nil);
    }
  };
  #endif

  static void Test_Tr_RB_Tree() 
  {
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("Traversable Red-Black Tree");
    SPG_WARN("-------------------------------------------------------------------------");
    #ifdef RBTREE_BASE_TRAVERSABLE
      SPG_WARN("RBTREE_BASE_TRAVERSABLE not defined ");
    #else
    {
      std::vector<std::pair<const int,int>> vals {{2,200},{11,1100},{4,400},{125,12500},{15,1500},{3,300},{9,900},{32,3200},{71,7100},{43,4300},{27,2700},{1,100}};

      Geom::RBTreeTraversable<int,int> tree(vals);
      tree.Validate();

      std::vector<int> vals2 {1,5,3,9,2};
      using unmapped_tree = Geom::RBTreeTraversable<int,void>;
      unmapped_tree tree2(vals2);

      SPG_WARN("TRAVERSAL AND ITERATING ")
      std::vector<std::pair<const int,int>> elements_out;
      SPG_INFO("In Order Traverse");
      tree.InOrderTraverse(elements_out);
      for(auto& [key,val]: elements_out) {
        SPG_TRACE("[{},{}]", key, val);
      }  


      auto itr = tree.Root();
      SPG_TRACE("Root node holds: {}", itr->second);
      auto left = tree.LeftChild(itr);
      SPG_TRACE("Left child holds: {}", left->second);
      auto right = tree.RightChild(itr);
      SPG_TRACE("Right child holds: {}", right->second);
      auto parent_l = tree.Parent(left);
      SPG_TRACE("Parent of left child holds: {}", parent_l->second);
      auto parent_r = tree.Parent(right);
      SPG_TRACE("Parent of right child holds: {}", parent_r->second);
    }
    #endif
  }
}