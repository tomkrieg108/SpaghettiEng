#include "RBTree.h"
#include "GeomUtils.h"
#include <functional>

/*
Red-Black Trees
  Intro
  https://www.youtube.com/watch?v=GmnODOF6Y8s

  Rotations:
  https://www.youtube.com/watch?v=yiM8VlIJwms

  Insertion:
  https://www.youtube.com/watch?v=yNP5OZGnQsw&t=332s

  Deletion: 
  https://www.youtube.com/watch?v=eoQpRtMpA9I

  https://www.programiz.com/dsa/deletion-from-a-red-black-tree

  https://medium.com/analytics-vidhya/deletion-in-red-black-rb-tree-92301e1474ea

  https://www.cs.purdue.edu/homes/ayg/CS251/slides/chap13c.pdf
*/


namespace Geom
{
  /***********************
  * Public functions
  */
 
  RBTree::RBTree()
  {
    m_nil = new RBTNode();
    m_nil->parent = m_nil->left=m_nil->right = m_nil;
    m_nil->colour = RBTree::Colour::Black;
    m_nil->value = FLT_MAX;
    m_root = m_nil;    
  }

  RBTree::RBTree(const std::vector<float>& values) : RBTree()
  {
    for(auto& val : values) 
      Insert(val);
  }

  RBTree::~RBTree()
  {
    Clear();
  }
  
  //Todo - return iterator to inserted element on success, end() on failure
  bool RBTree::Insert(float value)
  {
    if(m_root == m_nil) {
      m_root = CreateNode(value, m_nil);
      InsertFixup(m_root); //Todo - not necessary InsertFix will just set to black and return - already black
      return true;
    }

    RBTNode* cur = m_root;   
    while(true) {

      if(Geom::Equal(value, cur->value)) {
        SPG_ERROR("Failed to insert: {}.  Due to {} already inserted", value, cur->value);
        return false;
      }

      if( value < cur->value ) {
        if(cur->left == m_nil) {
          cur->left = CreateNode(value, cur);
          InsertFixup(cur->left);
          return true;
        } else {
          cur = cur->left;
        }
      }
      else {
        if(cur->right ==  m_nil) {
          cur->right = CreateNode(value, cur);
          InsertFixup(cur->right);
          return true;
        } else {
          cur = cur->right;
        }
      }
    }
  }

  void RBTree::Erase(float value)
  {
    auto itr = Find(value);
    if(itr == end()) {
       SPG_ERROR("Failed to erase: {}.  Not found", value);
       return;
    }
    //EraseWithoutFixup(itr.m_node);
    Erase(itr.m_node);
  }

  RBTree::Iterator RBTree::Find(float value)
  {
    RBTNode* node = m_root;
    while( (node != m_nil) && !Geom::EqualRel(value, node->value))  {
      if(value < node->value) 
        node = node->left;
      else if (value > node->value) 
        node = node->right;  
    }
    Iterator itr(*this, node);
    return itr;
  }

  //Todo - not yet implemented
  void RBTree::Clear()
  {

  }

  bool RBTree::Contains(float value)
  {
    auto itr = Find(value);
    return itr.m_node != m_nil;
  }

  uint32_t RBTree::Size()
  {
    return m_node_count;
  }

  bool RBTree::Empty()
  {
    return (m_node_count == 0);
  }

  std::vector<float> RBTree::InOrderTraverse()
  {
    std::vector<float> output;
    InOrderTraverse(m_root, output);
    return output;   
  }
  
  RBTree::Iterator RBTree::begin()
  {
    auto node = Min(m_root);
    Iterator itr(*this, node);
    return itr;
  } 

  RBTree::Iterator RBTree::end()
  {
    Iterator itr(*this, m_nil);
    return itr;
  }

  RBTree::Iterator RBTree::LowerBound(float key)
  {
    //Return iterator to the first element in tree which does not strictly go before value (i.e. either the element is equivalent to value or it goes after value). i.e. Comp(element, value) = false
    //Where do elements stop being strictly less than key 
    auto node = m_root;
    auto candidate_node = m_nil;
    while(node != m_nil) {

      //note: std::less<> is a class, std::less<>() instantiates an object of the class. std::less<>()(node->value, key) calls the () operator on the object.  The type of the parameters are deduced automatically

      // if(!std::less<>()(node->value, key) && !std::less<>()(key, node->value)) //equivalence
      if(Geom::Equal(node->value, key))
        return Iterator(*this,node);

      if(!std::less<>()(node->value, key)) {
        if( (candidate_node == m_nil) || (node->value < candidate_node->value))
          candidate_node = node;
      }
      if(key < node->value)
        node = node->left;
      else
        node = node->right;  
    }
    return Iterator(*this,candidate_node); 
  }

  RBTree::Iterator RBTree::UpperBound(float key)
  {
    //Return iterator to the first element in tree goes after value - i.e. Comp(value, element) = true, meaning must come before element
    //Where do elements start being strictly greater than value.
    auto node = m_root;
    auto candidate_node = m_nil;
    while(node != m_nil) {
      if(std::less<>()(key, node->value)) { 
        if( (candidate_node == m_nil) || (node->value < candidate_node->value))
          candidate_node = node;
      }
      if(key < node->value)
        node = node->left;
      else
        node = node->right;  
    }
    return Iterator(*this,candidate_node); 
  }

  void RBTree::Validate()
  {
    uint32_t node_count = 0;
    std::vector<uint32_t> black_depths, tree_depths;
    Validate(m_root, 0, 0, black_depths, tree_depths, node_count);
    SPG_WARN("VALIDATING RED-BLACK TREE----------------------------");
    SPG_INFO("  Total Nodes {}: ", node_count);
    SPG_INFO("  Tree Size is {} ", Size());
    SPG_INFO("  Max Black Depth from root: {}", *(std::max_element(black_depths.begin(), black_depths.end())) );
    SPG_INFO("  Min Black Depth from root: {}", *(std::min_element(black_depths.begin(), black_depths.end())) );
    SPG_INFO("  Max Tree Depth: {}", *(std::max_element(tree_depths.begin(), tree_depths.end())) );
    SPG_INFO("  Min Tree Depth: {}", *(std::min_element(tree_depths.begin(), tree_depths.end())) );
    SPG_INFO("  Path Count {}:", tree_depths.size());

    auto max = *(std::max_element(black_depths.begin(), black_depths.end()));
    auto min = *(std::min_element(black_depths.begin(), black_depths.end()));
    if(min != max)
      SPG_ERROR("Min/Max black black depth from root: {},{}", min,max);

    //Ordering check
    auto op_vals = InOrderTraverse();
    float cur = FLT_MIN;
    for(auto val : op_vals) {
      SPG_ASSERT(val > cur);
      //SPG_TRACE( "{},", val);
      cur = val;
    } 
    SPG_INFO("--ORDERING CHECK DONE--------------------------");
  
    BlackHeights black_heights;
    PathStack path_stack;
    Validate(m_root, black_heights, path_stack);
    int max_height = -1;
    for(auto& [node, black_heights] : black_heights) {
      if(node != m_nil) {
        // SPG_INFO("Node: {}", node->value);
        // for(auto h : black_heights) {
        //   SPG_TRACE("  {}", h);
        // }
        auto max = *(std::max_element(black_heights.begin(), black_heights.end()));
        auto min = *(std::min_element(black_heights.begin(), black_heights.end()));
        if(min != max)
          SPG_ERROR("Node: {}: Min/Max black height: {},{}", node->value, min,max);
        //SPG_ASSERT(max == min);
        if(max_height < max)
          max_height = max;
      }
    }
    SPG_INFO("MAX BLACK HEIGHT: {}", max_height);
    SPG_INFO("--BLACK HEIGHT CHECK PER NODE DONE--------------------------");
    SPG_WARN("FINISHED VALIDATING RED-BLACK TREE ----------------------------");
  }


  /*************************************************************
   * Private helper functions
  */

  RBTree::RBTNode* RBTree::CreateNode(float value, RBTNode* parent)
  {
    RBTNode* node = new RBTNode(value);
    node->parent = parent;
    node->left = node->right = m_nil;
    m_node_count++;
    return node;
  }

  void RBTree::DeleteNode(RBTNode* node)
  {
    SPG_ASSERT(node != nullptr);
    SPG_ASSERT(node != m_nil);
    delete node;
    m_node_count--;
  }

  inline bool RBTree::IsLeftChild(RBTNode* node)
  {
    if(node == m_root)  
      return false;
    return 
      node->parent->left == node;    
  }

  inline bool RBTree::IsRightChild(RBTNode* node)
  {
    if(node == m_root)  
      return false;  
    return 
      node->parent->right == node;    
  }

  uint32_t RBTree::NumChildren(RBTNode* node)
  {
    uint32_t count = 0;
    if(node->left != m_nil)
      ++count;
    if(node->right != m_nil)
      ++count;  
    return count;  
  }

  inline RBTree::RBTNode* RBTree::GrandParent(RBTNode* node)
  {
    if(node->parent != m_nil)
      return node->parent->parent;
    else
      return m_nil;  
  }

  RBTree::RBTNode* RBTree::Uncle(RBTNode* node)
  {
    RBTNode* g = GrandParent(node);
    if( (g != m_nil) && IsLeftChild(node->parent) )
      return g->right;
    if( (g != m_nil) && IsRightChild(node->parent) )
      return g->left;  
    return m_nil;   
  }

  RBTree::RBTNode* RBTree::Sibling(RBTNode* node)
  {
    SPG_ASSERT(node != m_root); 
    return IsLeftChild(node) ? node->parent->right : node->parent->left;
  }

  RBTree::RBTNode* RBTree::Min(RBTNode* node)
  {
    if(node == m_nil)
      return m_nil;
    while( (node->left != m_nil)  )   
      node = node->left;
    return node;  
  }

  RBTree::RBTNode* RBTree::Max(RBTNode* node)
  {
    if(node == m_nil)
      return m_nil;
    while( (node->right != m_nil)  )   
      node = node->right;
    return node;  
  }

  RBTree::RBTNode* RBTree::Next(RBTNode* node)
  {
    if(node == m_nil)
      return m_nil;
    if( node->right != m_nil)
      return Min(node->right);
    
    while(node->parent != m_nil) {
      if(IsLeftChild(node))
        return node->parent;
      else 
        node = node->parent;  
    }
    return m_nil;  
  }

  RBTree::RBTNode* RBTree::Previous(RBTNode* node)
  {
    if(node == m_nil)
      return m_nil;
    if( node->left != m_nil)
      return Max(node->left);
  
    while(node->parent != m_nil) {
      if(IsRightChild(node))
        return node->parent;
      else 
        node = node->parent;  
    }
    return m_nil;  
  }

  void RBTree::Transplant(RBTNode* u, RBTNode* v)
  {  //subtree u is replaced with subtree v (v is a descendent node of u).

    // if(u->parent == m_nil)
    //   m_root = v;
    // else if( u == u->parent->left)
    //   u->parent->left = v;
    // else
    //   u->parent->right = v;
    // v->parent = u->parent;  

    if(u == m_root)
      m_root = v;
    else if(IsRightChild(u))  
      u->parent->right = v;
    else
      u->parent->left = v;
      
    v->parent = u->parent;
  }

  void RBTree::SpliceOut(RBTNode* node)
  { //Splice out node from the tree (at most 1 non-null child).
    if(node == m_nil)
      return;

    SPG_ASSERT( !((node->left != m_nil) && (node->right != m_nil)) ); //todo - triggered sometimes
    if( (node->left == m_nil) && (node->right == m_nil)) { 
      Transplant(node, node->right);   //m_nil->parant = node->parent
    }
    else if(node->right == m_nil) {
      Transplant(node, node->left);   
    }
    else if(node->left == m_nil) {
      Transplant(node,node->right);
    }
  }

  void RBTree::Replace(RBTNode* node, RBTNode* replacement)
  {
    if(node == m_nil)
      return;

    //if replacement is nil, node should have no children
    if(replacement == m_nil) 
      SPG_ASSERT( (node->left == m_nil) && (node->right == m_nil)); //todo - triggered sometimes
    
    Transplant(node,replacement); 
    if(replacement != m_nil) {
      replacement->left = node->left;
      replacement->right = node->right;

      node->left->parent = replacement;
      node->right->parent = replacement;
    }

    //todo - if replacement is null this is a problem!
    //node->left->parent = replacement;
    //node->right->parent = replacement;
  }

  void RBTree::EraseWithoutFixup(RBTNode* node)
  {
    if(node == m_nil)
      return;

    //try this ------------------------
    //Seems to work!
    // RBTNode* r = nullptr;
    // if( (node->left == m_nil) && (node->right == m_nil) ) {
    //   r = node->right; //nil
    //   r->parent = node; //temporatily set parent of r to node, so that r can access it's correct parent
    // }
    // else if(node->left == m_nil) 
    //   r = node->right;
    // else if(node->right == m_nil) 
    //   r = node->left; 
    // else
    //   r = Min(node->right);

    // SpliceOut(r); //won't work if r is m_nil and it's parent still points to itself
    // Replace(node,r);
    // DeleteNode(node);
    // m_nil->parent = m_nil; //reset parent of m_nil to itself

    //-------------------------------------
    //original version - works

    if(NumChildren(node) < 2) 
      SpliceOut(node);
    else {  
      RBTNode* successor = Min(node->right);
      SpliceOut(successor);
      Replace(node,successor);
    } 
    DeleteNode(node);
  }

  std::string RBTree::PrintColour(RBTNode* node)
  {
    if(node->colour == Colour::Red)
      return "RED";
    else
      return "BLACK";
  }

  void RBTree::Erase(RBTNode* node)
  {
    if(node == m_nil)
      return;
    RBTNode* r = m_nil; //replacement
    RBTNode* x = m_nil; //replacement's right child if replacement not nil
    
    if( (node->left == m_nil) && (node->right == m_nil) ) {
      //SPG_ERROR("{} ({}) HAS 2 NIL CHILDREN", node->value, PrintColour(node));
      if(node->colour == Colour::Black) {
        int a =1;
      }else {
        int b = 1;
      }
      //Todo - after calling Replace() below, x->parent is set back to NIL.  Doesn't matter if node is Red, because nothing else is done.  Does screw it up it node is black
      m_nil->parent = node; //temporatily set parent of m_nil to node, so that its correct parent can be accessed
      r = x = m_nil; 
    }
    else if(node->left == m_nil) 
      r = x = node->right;
    else if(node->right == m_nil) 
      r = x = node->left; 
    else {
      r = Min(node->right); //replacement is node's successor
      SPG_ASSERT(r != m_nil); //cannot be nil since node has 2 non-nil children!
      SPG_ASSERT(r->left == m_nil);
      x = r->right; 
      if(x == m_nil)
        m_nil->parent = r; //temporatily set parent of m_nil to r so that its correct parent can be accessed
    }  

    if(r != m_nil)
      SpliceOut(r); 
    Replace(node,r); 

    if( (node->colour == Colour::Red) && ((r == m_nil) || (r->colour == Colour::Red)) ) {
      //nothin to do!
    }
    else if( (node->colour == Colour::Red) && (r != m_nil) && (r->colour == Colour::Black) ) {
      r->colour = Colour::Red;
      DeleteFixup(x);
    }
    else if( (node->colour == Colour::Black) && (r->colour == Colour::Red) ) {
      r->colour = Colour::Black;
    }
    else if( (x==m_root) && (node->colour == Colour::Black) && (r->colour == Colour::Black) ) {
      // Todo - can this even ever happen based on how x is set above?
      SPG_ASSERT(false);
    }
    else if((x!=m_root) && (node->colour == Colour::Black) &&  (r->colour == Colour::Black)) {
      DeleteFixup(x);
    }
    else
      SPG_ASSERT(false);

    DeleteNode(node);
    m_nil->parent = m_nil; //reset parent of m_nil to itself
  }

  void RBTree::InOrderTraverse(RBTNode* node, std::vector<float>& values_out)
  {
    if(node == m_nil)
      return;
    InOrderTraverse(node->left, values_out);
    values_out.push_back(node->value);
    InOrderTraverse(node->right,values_out);
  }
  
  void RBTree::RotateLeft(RBTNode* node)
  {
    SPG_ASSERT(node != m_nil);
    SPG_ASSERT(node->right != m_nil); //todo - triggered sometimes
    auto y = node->right; // right child 

    node->right = y->left;
    if(y->left != m_nil)
      y->left->parent = node;

    y->parent = node->parent;  
    if(IsLeftChild(node))
      node->parent->left = y;
    else if(IsRightChild(node))
      node->parent->right = y;
    else
      m_root = y;  

    y->left = node; 
    node->parent = y;
  }

  void RBTree::RotateRight(RBTNode* node)
  {
    SPG_ASSERT(node != m_nil);
    SPG_ASSERT(node->left != m_nil);
    auto y = node->left;

    node->left = y->right;
    if(y->right != m_nil)
      y->right->parent = node;

    y->parent = node->parent;
    if(IsLeftChild(node))
      node->parent->left = y;
    else if(IsRightChild(node))
      node->parent->right = y;
    else 
      m_root = y;

    y->right = node;
    node->parent = y;
  }

  void RBTree::InsertFixup(RBTNode* node) 
  {
    if(node ==  m_nil)
      return;
    
    auto cur = node;  
    while(true) {

      if(cur == m_root) {
        cur->colour = RBTree::Colour::Black;
        return;
      }

      if(cur->parent->colour == RBTree::Colour::Black)
        return; //nothing more to fix

      auto uncle = Uncle(cur);
      auto grandparent = GrandParent(cur);
      //case 1
      if(IsLeftChild(cur->parent) && (uncle != m_nil) && (uncle->colour == RBTree::Colour::Red) )  {
        cur->parent->colour = RBTree::Colour::Black;
        uncle->colour = RBTree::Colour::Black;
        grandparent->colour = RBTree::Colour::Red;
        cur = grandparent; //new grandparent is red and may need fixing - continue up tree
      }
      //case 2 (same steps as case 1 to fix)
      else if(IsRightChild(cur->parent) && (uncle != m_nil) && (uncle->colour == RBTree::Colour::Red) ) {
        cur->parent->colour = RBTree::Colour::Black;
        uncle->colour = RBTree::Colour::Black;
        grandparent->colour = RBTree::Colour::Red;
        cur = grandparent; //new grandparent is red and may need fixing - continue up tree
      }
      //case 3 (left,left)
      else if(IsLeftChild(cur) && IsLeftChild(cur->parent) && ((uncle == m_nil) || (uncle->colour == RBTree::Colour::Black))) {
        cur->parent->colour = RBTree::Colour::Black;
        SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
        grandparent->colour = RBTree::Colour::Red;
        RotateRight(grandparent); 
        return; //new grandparent is black - nothing more to fix
      }
      //case 4 (right, right)
      else if(IsRightChild(cur) && IsRightChild(cur->parent) && ((uncle == m_nil) || (uncle->colour == RBTree::Colour::Black))) {
        cur->parent->colour = RBTree::Colour::Black;
        SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
        grandparent->colour = RBTree::Colour::Red;
        RotateLeft(grandparent); //new grandparent is black - nothing more to fix
        return;
      }
      //case 5 (left, right)
      else if(IsRightChild(cur) && IsLeftChild(cur->parent) && ((uncle == m_nil) || (uncle->colour == RBTree::Colour::Black))) {
        RotateLeft(cur->parent); 
        cur = cur->left; //changes it to case 3 - fix in next iteration
      }
      //case 6 (right, left)
      else if(IsLeftChild(cur) && IsRightChild(cur->parent) && ((uncle == m_nil) || (uncle->colour == RBTree::Colour::Black))) {
        RotateRight(cur->parent); 
        cur = cur->right; //changes it to case 4 - fix in next iteration
      }
      else
        SPG_ASSERT(false);
    }
  }

  void RBTree::DeleteFixup(RBTNode* x)
  {
    RBTNode* w = Sibling(x);
  
    if(x->colour == RBTree::Colour::Red) {
      DeleteFixup_0(x);
    }
  
    else if( (x->colour == RBTree::Colour::Black) && (w->colour == RBTree::Colour::Red) ) {
      DeleteFixup_1(x);
    }

    else if( (x->colour == RBTree::Colour::Black) && (w->colour == RBTree::Colour::Black) ) {

      if ( (w->left->colour == RBTree::Colour::Black) && (w->right->colour == RBTree::Colour::Black) ) {
        DeleteFixup_2(x);
      }
      else if(IsLeftChild(x) && (w->left->colour == Colour::Red) && (w->right->colour == Colour::Black) ) {
        DeleteFixup_3(x);
      } 
      else if(IsRightChild(x) && (w->right->colour == Colour::Red) && (w->left->colour == Colour::Black) ) {
        DeleteFixup_3(x);
      } 
      else if(IsLeftChild(x) && (w->right->colour == Colour::Red)) {
        DeleteFixup_4(x);
      }
      else if(IsRightChild(x) && (w->left->colour == Colour::Red)) {
        DeleteFixup_4(x);
      }
      else
        SPG_ASSERT(false);
    }
    else
      SPG_ASSERT(false);
  } 

  void RBTree::DeleteFixup_0(RBTNode* x)
  {
    x->colour = RBTree::Colour::Black;
  }

  void RBTree::DeleteFixup_1(RBTNode* x)
  {
    RBTNode* w = Sibling(x);
    w->colour = Colour::Black;
    x->parent->colour = Colour::Red;
    if(IsLeftChild(x)) {
      RotateLeft(x->parent);
      //w = x->parent->right; //i.e. w = Sibling(x);
    }
    else {
      RotateRight(x->parent);   
      //w = x->parent->left; // i.e. w = Sibling(x);
    }
    w = Sibling(x); //not really needed - w will be set from x in the next call to DeleteFixup_*()
    DeleteFixup(x);
  }

  void RBTree::DeleteFixup_2(RBTNode* x)
  {
    RBTNode* w = Sibling(x);
    w->colour = RBTree::Colour::Red;
    x = x->parent;  
    if(x->colour == Colour::Red)
       DeleteFixup_0(x);
    else {
      if(x == m_root)
        return;
      DeleteFixup(x);
    }
  }

  void RBTree::DeleteFixup_3(RBTNode* x)
  {
    RBTNode* w = Sibling(x);
    if(IsLeftChild(x)) 
      w->left->colour = Colour::Black;
    else 
      w->right->colour = Colour::Black;
    
    w->colour = Colour::Red;
    
    if(IsLeftChild(x)) 
      RotateRight(w);
    else
      RotateLeft(w);

    //i.e. w = Sibling(x);
    // if(IsLeftChild(x)) 
    //   w = x->parent->right;
    // else
    //   w = x->parent->left;
      
    w = Sibling(x);  //not really needed - w will be set from x in the next call to DeleteFixup_*()
    DeleteFixup_4(x);  
  }

  void RBTree::DeleteFixup_4(RBTNode* x)
  {
    RBTNode* w = Sibling(x);
    w->colour = x->parent->colour;
    x->parent->colour = Colour::Black;
    if(IsLeftChild(x)) {
      w->right->colour = Colour::Black;
      RotateLeft(x->parent);
    }
    else {
      w->left->colour = Colour::Black;
      RotateRight(x->parent);   
    }
  }

  void RBTree::Validate(RBTNode* node, uint32_t black_depth, uint32_t depth, 
    std::vector<uint32_t>& black_depths, std::vector<uint32_t>& depths, uint32_t& node_count)
  {
    if(node == m_nil) {
      SPG_ASSERT(node->colour == Colour::Black);
      black_depths.push_back(black_depth);
      depths.push_back(depth);
      return;
    }

    if(node == m_root) {
      SPG_ASSERT(node->parent == m_nil);
      SPG_ASSERT(node->colour == Colour::Black);
    }

    if(node->colour == Colour::Red) {
      SPG_ASSERT(node->left->colour == Colour::Black);
      SPG_ASSERT(node->right->colour == Colour::Black);  
    }

    if(node->left != m_nil)
      SPG_ASSERT(node->left->value < node->value);
    if(node->right != m_nil)
      SPG_ASSERT(node->right->value > node->value);  

    node_count++;  
      
    if(node->colour == RBTree::Colour::Black)  {
      Validate(node->left, black_depth+1, depth+1, black_depths, depths, node_count);
      Validate(node->right, black_depth+1, depth+1, black_depths, depths, node_count);
    }
    else {
      Validate(node->left, black_depth, depth+1, black_depths, depths, node_count);
      Validate(node->right, black_depth, depth+1, black_depths, depths, node_count);  
    }
  }

  void RBTree::Validate(RBTNode* node, BlackHeights& black_heights, PathStack& path_stack)
  {
    path_stack.push_back(node);
   
    //Add a new element into the BlackHeights map for this node if none already exists. Initialize to an empty vector
    if( black_heights.find(node) == black_heights.end()) {
      std::vector<int> depths;
      black_heights.insert({node, depths});
    }
   
    if(node == m_nil) { //end of path
      SPG_ASSERT(path_stack[0] == m_root);
      int black_height = 0;
      for(auto r_itr = path_stack.rbegin(); r_itr != path_stack.rend(); r_itr++ ) {
        RBTNode* n = *r_itr;
        if( (n->colour == Colour::Black ) && (n != m_nil))
          ++black_height;
        black_heights[n].push_back(black_height);
      }
    }
    else {
      Validate(node->left,black_heights,path_stack);
      Validate(node->right,black_heights,path_stack);
    }

    path_stack.pop_back();
  }


}