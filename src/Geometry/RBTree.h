#pragma once
#include <Common/Common.h>
#include <functional> //std::less

//RBTREE INFO:
/*
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

  ChatGPT: "Range tree implementation review", "C++ Map/Set Value Traits"
*/

namespace Geom
{
  #define RBTREE_VERS 2
  

#if (RBTREE_VERS==1)
    
  #define RBT_TEMPLATE template<typename TKey, typename ValueTraits, typename TComp>
  #define RBT_TYPE RBTree<TKey,ValueTraits,TComp>

  template<
    typename TKey, 
    typename ValueTraits, 
    typename TComp = std::less<TKey>>
  class RBTree
  {
    friend class RangeTree1D;
    friend class RangeTree2D;

  private:  
    using key_type =  TKey;
    using value_type = typename ValueTraits::value_type;
    using reference = value_type&; 
    using const_reference = const value_type&;
    using pointer = value_type*;
    using comparator_type = TComp;

    enum class Colour : uint16_t {Red, Black};
    
  private:
    
    struct RBNode
    {
      RBNode* left = nullptr;
      RBNode* right = nullptr;
      RBNode* parent = nullptr;
      Colour colour = Colour::Red;
      value_type value;

      RBNode() = default;

      template<typename... Args>
      RBNode(Args&&... args) : value(std::forward<Args>(args)...) {}
    };
  
  public:  

    class Iterator
    {
    private:
      using iterator_category = std::forward_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      
    public:
      Iterator& Next() {
        if(m_node == m_nil)
          return *this;
        if( m_node->right != m_nil) {  //find min of right tree
          m_node = m_node->right;
          while( (m_node->left != m_nil)  )   
            m_node = m_node->left;
          return *this;  
        }
        while(m_node->parent != m_nil) {
          auto m_node_save = m_node;
          m_node = m_node->parent;
          if(m_node_save->parent->left == m_node_save) //m_node is a left child
            return *this;
        }
        m_node = m_nil;
        return *this;  
      }
      Iterator& Prev()
      {
        if(m_node == m_nil)
          return *this;
        if( m_node->left != m_nil) { //find max of left tree
          m_node = m_node->left;
          while( (m_node->right != m_nil)  )   
            m_node = m_node->right;
          return *this;  
        }
        while(m_node->parent != m_nil) {
          auto m_node_save = m_node;
          if(m_node_save->parent->right == m_node_save) //m_node is a left child
            return *this;
        }
        m_node = m_nil;
        return *this;  
      }
      Iterator& operator++ () { 
        return Next();
      }
      bool operator == (const Iterator& other) const {
        return (m_node == other.m_node) && (m_nil == other.m_nil);
      }
      bool operator != (const Iterator& other) const {
        return !(*this == other);
      }
      reference operator* () const { 
        SPG_ASSERT(m_node != nullptr); 
        return m_node->value;
      }
      pointer operator->() const {
        return &(operator*());
      }
    
    private:
      friend class RBTree;
      Iterator() = default;
      Iterator(RBNode* node, RBNode* nil) : m_node{node}, m_nil{nil} {}
      RBNode* m_node;
      RBNode* m_nil;
    };

  public:
    RBTree(TComp comp = TComp());
    RBTree(const std::vector<value_type>& elements, TComp comp = TComp());
    RBTree(const RBTree& other);
    RBTree(RBTree&& other) noexcept;
    RBTree& operator = (const RBTree& other);
    RBTree& operator = (RBTree&& other) noexcept;
    ~RBTree();
  
    bool Insert(const value_type& element);
    bool Erase(const TKey& key);
    void Clear();
    bool Contains(const TKey& key);
    uint32_t Size();
    bool Empty();
    void InOrderTraverse(std::vector<value_type>& values_out); //Flatten() return the vec
    
    Iterator begin();
    Iterator end();
    Iterator Find(const TKey& key);
    Iterator LowerBound(const TKey& key);
    Iterator UpperBound(const TKey& key);

    //Todo - only used for RangeTree
    RBNode* FindSplitNode(const TKey& key_low, const TKey& key_high);

    void Validate();
  
  private:
    void InitSentinal();
    RBNode* CreateNode(const value_type& element, RBNode* parent);
    void DeleteNode(RBNode* node);
    bool IsLeaf(RBNode* n) {
      return (n->left == m_nil) && (n->right == m_nil); //m_nil will also be a leaf
    }
    bool IsLeftChild(RBNode* node);
    bool IsRightChild(RBNode* node);
    uint32_t NumChildren(RBNode* node);
    RBNode* GrandParent(RBNode* node);
    RBNode* Uncle(RBNode* node);
    RBNode* Sibling(RBNode* node);
    RBNode* Min(RBNode* node);
    RBNode* Max(RBNode* node);
    void SpliceOut(RBNode* node); 
    void Replace(RBNode* node, RBNode* replacement);
    void Transplant(RBNode* u, RBNode* v);
    void RotateLeft(RBNode* node);
    void RotateRight(RBNode* node);
    void InOrderTraverse(RBNode* node, std::vector<value_type>& values_out); 

    void EraseWithoutFixup(RBNode* node); 
    void Erase(RBNode* node);
    
    void InsertFixup(RBNode* node);
    void DeleteFixup(RBNode* x);
    void DeleteFixup_0(RBNode* x);
    void DeleteFixup_1(RBNode* x);
    void DeleteFixup_2(RBNode* x);
    void DeleteFixup_3(RBNode* x);
    void DeleteFixup_4(RBNode* x);
    
    void Validate(RBNode* node, uint32_t black_depth, uint32_t depth, 
      std::vector<uint32_t>& black_depths, std::vector<uint32_t>& depths, uint32_t& node_count);

    using BlackHeights = std::unordered_map<RBNode*, std::vector<int>>;
    using PathStack = std::vector<RBNode*>;
    void Validate(RBNode* node, BlackHeights& black_heights, PathStack& path_stack);
  
    bool Equal(const TKey& k1, const TKey& k2) { return !m_comp(k1,k2) && !m_comp(k2,k1);}
    bool Less(const TKey& k1, const TKey& k2) { return m_comp(k1,k2);}
    TKey Key(const value_type& value) {return ValueTraits::Key(value);}

  private:
    RBNode* m_root;
    uint32_t m_node_count = 0;
    RBNode* m_nil;
    comparator_type m_comp;
  };


  RBT_TEMPLATE
  RBT_TYPE::RBTree(TComp comp) : m_comp(comp)
  {
    InitSentinal();
  }

  RBT_TEMPLATE
  RBT_TYPE::RBTree(const std::vector<value_type>& elements, TComp comp) : m_comp(comp)
  {
    InitSentinal();
    for(auto& e : elements) 
      Insert(e);
  }

  RBT_TEMPLATE
  RBT_TYPE::RBTree(const RBTree& other)
  {
    InitSentinal();
    for(auto& element : const_cast<RBTree&>(other)) {
      Insert(element);
    }  
  }

  RBT_TEMPLATE
  RBT_TYPE::RBTree(RBTree&& other) noexcept {
    m_root = other.m_root;
    m_nil = other.m_nil;
    m_node_count = other.m_node_count;
    other.InitSentinal();  //keep the sentinal node in other
  }

  RBT_TEMPLATE
  RBT_TYPE & RBT_TYPE::operator = (const RBTree& other)
  {
    if(&other != this) {
      Clear();
      for(auto& element : const_cast<RBTree&>(other))
        Insert(element);
    }
    return *this;
  }

  RBT_TEMPLATE
  RBT_TYPE& RBT_TYPE::operator = (RBTree&& other) noexcept 
  {
    if(&other != this) {
      m_root = other.m_root;
      m_nil = other.m_nil;
      m_node_count = other.m_node_count;
      other.InitSentinal();  //keep the sentinal node in other
    }
    return *this;
  }

  RBT_TEMPLATE
  RBT_TYPE::~RBTree()
  {
    Clear();
    delete m_nil;
    m_nil = m_root = nullptr;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Insert(const value_type& element)
  {
    if(m_root == m_nil) {
      m_root = CreateNode(element, m_nil);
      InsertFixup(m_root); //Todo - not necessary InsertFix will just set to black and return - already black
      return true;
    }

    RBNode* cur = m_root;   
    while(true) {
      if(Equal(Key(element),Key(cur->value))) {
        SPG_ERROR("Failed to insert: {}.  Due to {} already inserted", Key(element), Key(cur->value));
        return false;
      }

      if(Less(Key(element),Key(cur->value))) {
        if(cur->left == m_nil) {
          cur->left = CreateNode(element, cur);
          InsertFixup(cur->left);
          return true;
        } else {
          cur = cur->left;
        }
      }
      else {
        if(cur->right ==  m_nil) {
          cur->right = CreateNode(element, cur);
          InsertFixup(cur->right);
          return true;
        } else {
          cur = cur->right;
        }
      }
    }
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Erase(const TKey& key)
  {
    auto itr = Find(key);
    if(itr == end()) {
      SPG_ERROR("Failed to erase: {}.  Not found", key);
      return false;
    }
    Erase(itr.m_node);
    return true;
  }

  RBT_TEMPLATE
  void RBT_TYPE::Clear()
  {
    while(m_node_count > 0)
      EraseWithoutFixup(m_root);
    m_root = m_nil;  
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Contains(const TKey& key)
  {
    auto itr = Find(key);
    return itr.m_node != m_nil;
  }

  RBT_TEMPLATE
  uint32_t RBT_TYPE::Size()
  {
    return m_node_count;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Empty()
  {
    return (m_node_count == 0);
  }

  RBT_TEMPLATE
  void RBT_TYPE::InOrderTraverse(std::vector<value_type>& values_out)
  {
    InOrderTraverse(m_root, values_out);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::begin()
  {
    auto node = Min(m_root);
    return Iterator(node,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::end()
  {
    return Iterator(m_nil,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Find(const TKey& key)
  {
    RBNode* node = m_root;
    while( (node != m_nil) && !Equal(key, Key(node->value)))  {
      if(Less(key,Key(node->value)))
        node = node->left;
      else if (Less(Key(node->value), key))
        node = node->right;  
    }
    Iterator itr(node, m_nil);
    return itr;
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::LowerBound(const TKey& key)
  {
      //Return iterator to the first element in tree which does not strictly go before key (i.e. either the element is equivalent to value or it goes after value). i.e. Comp(element.key, key) = false
    //Where do elements stop being strictly less than key 
    RBNode* node = m_root;
    RBNode* candidate_node = m_nil;
    while(node != m_nil) {
      if(Equal(Key(node->value), key))
        return Iterator(node, m_nil);
      if(!Less(Key(node->value), key)) {
        if( (candidate_node == m_nil) || Less(Key(node->value), Key(candidate_node->value)))
          candidate_node = node;
      }
      if(Less(key, Key(node->value)))
        node = node->left;
      else
        node = node->right;  
    }
    return Iterator(candidate_node, m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::UpperBound(const TKey& key)
  {
    //Return iterator to the first element in tree that goes after key - i.e. Comp(key, element.key) = true, meaning key must come before element
    //Where do elements start being strictly greater than value.
    RBNode* node = m_root;
    RBNode* candidate_node = m_nil;
    while(node != m_nil) {
      if(Less(key, Key(node->value))) { 
        if( (candidate_node == m_nil) || Less(Key(node->value), Key(candidate_node->value)))
          candidate_node = node;
      }
      if(Less(key,Key(node->value)))
        node = node->left;
      else
        node = node->right;  
    }
    return Iterator(candidate_node, m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::FindSplitNode(const TKey& key_low, const TKey& key_high)
  {
    //Todo Make sure Less() is consisten for open verses closed intervals
    SPG_ASSERT(Less(key_low,key_high));
    RBNode* node = m_root;
    while( !IsLeaf(node) && (!Less(node->value,key_high) || Less(node->value,key_low)) ) {
      if(!Less(node->value,key_high))
        node = node->left;
      else
        node = node->right;  
    }
    return node;
  }
  
  RBT_TEMPLATE
  void RBT_TYPE::Validate()
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

    BlackHeights black_heights;
    PathStack path_stack;
    Validate(m_root, black_heights, path_stack);
    int max_height = -1;
    for(auto& [node, black_heights] : black_heights) {
      if(node != m_nil) {
        auto max = *(std::max_element(black_heights.begin(), black_heights.end()));
        auto min = *(std::min_element(black_heights.begin(), black_heights.end()));
        if(min != max)
          SPG_ERROR("Node: {}: Min/Max black height: {},{}", Key(node->value), min,max);
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
  *************************************************************/
  RBT_TEMPLATE
  void RBT_TYPE::InitSentinal()
  {
    m_nil = m_root = nullptr;
    m_node_count = 0;
    m_nil = new RBNode();
    m_nil->parent = m_nil->left=m_nil->right = m_nil;
    m_nil->colour = RBTree::Colour::Black;
    m_root = m_nil;  
  }
  
  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::CreateNode(const value_type& element, RBNode* parent)
  {
    RBNode* node = new RBNode(element);
    node->parent = parent;
    node->left = node->right = m_nil;
    m_node_count++;
    return node;
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteNode(RBNode* node)
  {
    SPG_ASSERT(node != nullptr);
    SPG_ASSERT(node != m_nil);
    delete node;
    m_node_count--;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsLeftChild(RBNode* node)
  {
    return ( (node==m_root) ? false : (node->parent->left == node));
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsRightChild(RBNode* node)
  {
    return ( (node==m_root) ? false : (node->parent->right == node));
  }

  RBT_TEMPLATE
  uint32_t RBT_TYPE::NumChildren(RBNode* node)
  {
    uint32_t count = 0;
    if(node->left != m_nil)
      ++count;
    if(node->right != m_nil)
      ++count;  
    return count;  
  }

  RBT_TEMPLATE 
  RBT_TYPE::RBNode* RBT_TYPE::GrandParent(RBNode* node)
  {
    return ((node->parent == m_nil) ? m_nil : node->parent->parent );
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Uncle(RBNode* node)
  {
    RBNode* g = GrandParent(node);
    if( (g != m_nil) && IsLeftChild(node->parent) )
      return g->right;
    if( (g != m_nil) && IsRightChild(node->parent) )
      return g->left;  
    return m_nil;   
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Sibling(RBNode* node)
  {
    if(node == m_root)
      return m_nil;
    return IsLeftChild(node) ? node->parent->right : node->parent->left;
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Min(RBNode* node)
  {
    if(node == m_nil)
      return m_nil;
    while( (node->left != m_nil)  )   
      node = node->left;
    return node;  
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Max(RBNode* node)
  {
    if(node == m_nil)
      return m_nil;
    while( (node->right != m_nil)  )   
      node = node->right;
    return node;  
  }

  RBT_TEMPLATE
  void RBT_TYPE::Transplant(RBNode* u, RBNode* v)
  { //subtree u is replaced with subtree v (v is a descendent node of u).
    if(u == m_root)
      m_root = v;
    else if(IsRightChild(u))  
      u->parent->right = v;
    else
      u->parent->left = v;
    v->parent = u->parent;
  }

  RBT_TEMPLATE
  void RBT_TYPE::SpliceOut(RBNode* node)
  { //Splice out node from the tree (at most 1 non-null child).
    if(node == m_nil)
      return;
    SPG_ASSERT( !((node->left != m_nil) && (node->right != m_nil)) ); 
    if( (node->left == m_nil) && (node->right == m_nil))
      Transplant(node, node->right);   //m_nil->parant = node->parent
    else if(node->right == m_nil) 
      Transplant(node, node->left);   
    else if(node->left == m_nil) 
      Transplant(node,node->right);
  }

  RBT_TEMPLATE
  void RBT_TYPE::Replace(RBNode* node, RBNode* replacement)
  {
    if(node == m_nil)
      return;

    if(replacement == m_nil)  //if replacement is nil, node should have no children
      SPG_ASSERT( (node->left == m_nil) && (node->right == m_nil)); 
    
    Transplant(node,replacement); 
    if(replacement != m_nil) {
      replacement->left = node->left;
      replacement->right = node->right;

      node->left->parent = replacement;
      node->right->parent = replacement;
    }
  } 

  RBT_TEMPLATE
  void RBT_TYPE::RotateLeft(RBNode* node)
  {
    SPG_ASSERT(node != m_nil);
    SPG_ASSERT(node->right != m_nil);
    auto y = node->right; 

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

  RBT_TEMPLATE
  void RBT_TYPE::RotateRight(RBNode* node)
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

  RBT_TEMPLATE
  void RBT_TYPE::InOrderTraverse(RBNode* node, std::vector<value_type>& values_out)
  {
    if(node == m_nil)
      return;
    InOrderTraverse(node->left, values_out);
    values_out.push_back(node->value);
    InOrderTraverse(node->right,values_out);
  }

  RBT_TEMPLATE
  void RBT_TYPE::EraseWithoutFixup(RBNode* node)
  {
    if(node == m_nil)
      return;
    if(NumChildren(node) < 2) 
      SpliceOut(node);
    else {  
      RBNode* successor = Min(node->right);
      SpliceOut(successor);
      Replace(node,successor);
    } 
    DeleteNode(node);
  }

  RBT_TEMPLATE
  void RBT_TYPE::Erase(RBNode* node)
  {
    if(node == m_nil)
      return;
    RBNode* r = m_nil; //replacement
    RBNode* x = m_nil; //replacement's right child if replacement not nil
    
    if( (node->left == m_nil) && (node->right == m_nil) ) {
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
  
  RBT_TEMPLATE
  void RBT_TYPE::InsertFixup(RBNode* node)
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
      else if(IsLeftChild(cur) && IsLeftChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        cur->parent->colour = RBTree::Colour::Black;
        SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
        grandparent->colour = RBTree::Colour::Red;
        RotateRight(grandparent); 
        return; //new grandparent is black - nothing more to fix
      }
      //case 4 (right, right)
      else if(IsRightChild(cur) && IsRightChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        cur->parent->colour = RBTree::Colour::Black;
        SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
        grandparent->colour = RBTree::Colour::Red;
        RotateLeft(grandparent); //new grandparent is black - nothing more to fix
        return;
      }
      //case 5 (left, right)
      else if(IsRightChild(cur) && IsLeftChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        RotateLeft(cur->parent); 
        cur = cur->left; //changes it to case 3 - fix in next iteration
      }
      //case 6 (right, left)
      else if(IsLeftChild(cur) && IsRightChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        RotateRight(cur->parent); 
        cur = cur->right; //changes it to case 4 - fix in next iteration
      }
      else
        SPG_ASSERT(false);
    }
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup(RBNode* x)
  {
    RBNode* w = Sibling(x);

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

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_0(RBNode* x)
  {
      x->colour = RBTree::Colour::Black;
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_1(RBNode* x)
  {
    RBNode* w = Sibling(x);
    w->colour = Colour::Black;
    x->parent->colour = Colour::Red;
    if(IsLeftChild(x)) 
      RotateLeft(x->parent);
    else 
      RotateRight(x->parent);   
    w = Sibling(x); //not really needed - w will be set from x in the next call to DeleteFixup_*()
    DeleteFixup(x);
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_2(RBNode* x)
  {
    RBNode* w = Sibling(x);
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

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_3(RBNode* x)
  {
    RBNode* w = Sibling(x);
    if(IsLeftChild(x)) 
      w->left->colour = Colour::Black;
    else 
      w->right->colour = Colour::Black;
    w->colour = Colour::Red;
    if(IsLeftChild(x)) 
      RotateRight(w);
    else
      RotateLeft(w);
    w = Sibling(x);  //not really needed - w will be set from x in the next call to DeleteFixup_*()
    DeleteFixup_4(x); 
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_4(RBNode* x)
  {
    RBNode* w = Sibling(x);
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

  RBT_TEMPLATE
  void RBT_TYPE::Validate(RBNode* node, uint32_t black_depth, uint32_t depth, 
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
      SPG_ASSERT(Less(Key(node->left->value),Key(node->value)));
    if(node->right != m_nil)
      SPG_ASSERT(Less(Key(node->value),Key(node->right->value)));  

    node_count++;  
    black_depth += (node->colour == RBTree::Colour::Black) ? 1 : 0;
    
    Validate(node->left, black_depth, depth+1, black_depths, depths, node_count);
    Validate(node->right, black_depth, depth+1, black_depths, depths, node_count);
  }

  RBT_TEMPLATE
  void RBT_TYPE::Validate(RBNode* node, BlackHeights& black_heights, PathStack& path_stack)
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
        RBNode* n = *r_itr;
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

  /*************************************************************
  * Map
  *************************************************************/
  
  template<typename TKey, typename TVal>
  struct MapValueTraits
  {
    using value_type = std::pair<const TKey, TVal>;
    static const TKey& Key(const value_type& v) {return v.first;}
  };

  template<typename TKey>
  struct SetValueTraits
  {
    using value_type = TKey; 
    static const TKey& Key(const value_type& v) {return v;}
  };

  template<typename TKey, typename TVal, typename TComp = std::less<TKey>>
  using Map = RBTree<TKey, MapValueTraits<TKey, TVal>, TComp>;

  template<typename TKey, typename TComp = std::less<TKey>>
  using Set = RBTree<TKey, SetValueTraits<TKey>, TComp>;

  //Todo - Do this if want to extend the behaviour.  Change private to protected in RBTree
  // template<typename TKey, typename TypeTraits, typename TComp = std::less<TKey>>
  // class MyMap : public RBTree<TKey, MapValueTraits<TKey, TVal>, TComp = std::less<TKey>>
  // {
  //   using Base = RBTree<TKey, MapValueTraits, TComp = std::less<TKey>>;
  //   public:
  //     using typename Base::key_type;
  //     using typename Base::value_type;
  //     using typename Base::reference;
  //     using typename Base::const_reference;
  //     using typename Base::iterator;
  // };

  static void Test_RB_Tree() 
  {
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("Red-Black Tree V3 (Templated)");
    SPG_WARN("-------------------------------------------------------------------------");
    {
      
      std::vector<std::pair<const int,int>> map_vals {{2,200},{11,1100},{4,400},{125,12500},{15,1500},{3,300},{9,900},{32,3200},{71,7100},{43,4300},{27,2700},{1,100}};

      Geom::Map<int, int> my_map(map_vals);
      my_map.Validate();

      //Traversal and iterating
      {
        SPG_WARN("TRAVERSAL AND ITERATING ")
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse");
        my_map.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        
        // SPG_INFO("Ranged for loop traverse");
        // for(auto& [key,val] : my_map) {
        //   SPG_TRACE("[{},{}]", key, val);
        // }

        SPG_INFO("Loop using Iterator")
        for(auto itr = my_map.begin(); itr != my_map.end(); ++itr) {
          auto& [key,val] = *itr;
          SPG_TRACE("[{},{}]", key, val);
        }
      }

      //Find(), Contains()
      {
        SPG_WARN("FIND, CONTAINS ")
        for(auto& [key, value]: map_vals) {
          SPG_INFO("Contains {}? {}", key, my_map.Contains(key));
        }
        SPG_INFO("Contains {}? {}", -15, my_map.Contains(-15));
        SPG_INFO("Contains {}? {}", 0, my_map.Contains(0));
        SPG_INFO("Contains {}? {}", 160, my_map.Contains(160));
        SPG_INFO("Contains {}? {}", 33, my_map.Contains(33));
      }

      {
        SPG_WARN("LOWERBOUND UPPERBOUND ")
        for(auto& [key, value]: map_vals) {
          auto itr = my_map.LowerBound(key);
          if(itr == my_map.end()) {
            SPG_INFO("LowerBound {}? end", key);
          }
          else {
            SPG_INFO("LowerBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_map.LowerBound(k);
          if(itr == my_map.end()) {
            SPG_INFO("LowerBound {}? end", k);
          }
          else {
            SPG_INFO("LowerBound {}? {}", k, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          auto itr = my_map.UpperBound(key);
          if(itr == my_map.end()) {
            SPG_INFO("UpperBound {}? end", key);
          }
          else {
            SPG_INFO("UpperBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_map.UpperBound(k);
          if(itr == my_map.end()) {
            SPG_INFO("UpperBound {}? end", k);
          }
          else {
            SPG_INFO("UpperBound {}? {}", k, (*itr).first);
          }
        }
        
      }

      //Deletion
      {
        SPG_WARN("DELETION ")
        my_map.Erase(1);
        my_map.Erase(71);
        my_map.Erase(27);
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse After erase");
        my_map.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        my_map.Validate();
      }

      //Stress test with large data, insertion, erasure, clear
      {
        SPG_WARN("STRESS TEST WITH LARGE AMOUNT OF DATA ")
        my_map.Clear();
        //Add a bunch of random values
        std::vector<int> rb_values;
        std::vector<bool> rb_values_to_delete;
      
        const uint32_t RB_NUM_VALS = 10000;
        const int RB_MIN_VAL = -100000;
        const int RB_MAX_VAL = 100000;
    
        std::random_device rd;                         
        std::mt19937 mt(rd()); 
        std::uniform_int_distribution<int> dist(RB_MIN_VAL, RB_MAX_VAL); 
      
        for(int i=0; i< RB_NUM_VALS; i++) {
          int val = dist(mt);
          auto element = std::make_pair(val,val*100);
          if(my_map.Insert(element)) {
            rb_values.push_back(val);
            rb_values_to_delete.push_back(false);    
          }
        }
        my_map.Validate();

        //pick values to delete at random
        std::uniform_int_distribution<int> idist(0, rb_values.size()-1);  
        const uint32_t RB_NUM_VALS_TO_DEL = rb_values.size()/2;
        for(int i=0; i<RB_NUM_VALS_TO_DEL; i++ ) {
          int idx = 0;
          do {
            idx = idist(mt);
          } while (rb_values_to_delete[idx] == true);
          rb_values_to_delete[idx] = true;
        }
        
        SPG_INFO("Deleting {} values at random ", RB_NUM_VALS_TO_DEL);
        for(int i=0; i<rb_values_to_delete.size(); i++ ) {
          if(!rb_values_to_delete[i])
            continue;
          my_map.Erase(rb_values[i]);
        }
        my_map.Validate();
        SPG_INFO("Clearing Tree");
        my_map.Clear();
        my_map.Validate();
        SPG_INFO("Tree Size {}: ", my_map.Size());

        SPG_INFO("Add a few more elements");
        for(auto& element : map_vals)
          my_map.Insert(element);
        SPG_INFO("Ranged for loop traverse");
        for(auto& [key,val] : my_map) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("Tree Size {}: ", my_map.Size());
        my_map.Validate();
      }
    
      //Copy move constructor, assignment operator
      {
        auto t2 = my_map; //copy constructor
        Geom::Map<int,int> t3;
        t3 = t2; //copy assignment;
        SPG_INFO("t2:");
        for(auto& [key,val] : t2) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t3:");
        for(auto& [key,val] : t3) {
          SPG_TRACE("[{},{}]", key, val);
        }
        auto t4 = std::move(t2); //move ctr
        Geom::Map<int,int> t5;
        t5 = std::move(t3); //move assignment

        SPG_INFO("After move:");
        SPG_INFO("t2:");
        for(auto& [key,val] : t2) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t3:");
        for(auto& [key,val] : t3) {
          SPG_TRACE("[{},{}]", key, val);
        }

        SPG_INFO("t4:");
        for(auto& [key,val] : t4) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t5:");
        for(auto& [key,val] : t5) {
          SPG_TRACE("[{},{}]", key, val);
        }
      }

      {
        SPG_WARN("IN-ORDER TRAVERSAL OF BALANCED TREE ")
        std::vector<float> vals{26,32,43,11,15,100,17,7,87,42,150,111, 27, 54,1,33,200,88,99,0};
        std::vector<float> ordered_vals_out;
        Geom::Set<float> rb_tree;
        for(auto v : vals)
          rb_tree.Insert(v);
        rb_tree.InOrderTraverse(ordered_vals_out);
        SPG_WARN("Elements in RBTree {}", rb_tree.Size())
        for(auto& v : ordered_vals_out) {
          SPG_TRACE(v);
        }

      }
    }

 }
#endif
  

//===============================================================
// Ver 2
//===============================================================

#if (RBTREE_VERS==2)
  
  //#define RBTREE_BASE_TRAVERSABLE
  #define RBT_TEMPLATE template<typename TKey,typename TValue,typename TComp,typename Traits>
  #define RBT_TYPE RBTree<TKey,TValue,TComp,Traits>

  template<typename TKey, typename TValue = void>
  struct DefaultTraits;   //forward declaration — definition comes below

  template <typename TKey, typename TValue>
  struct DefaultTraits
  {
    using key_type = TKey;
    using value_type = std::pair<const TKey, TValue>;

    static const key_type& Key(const value_type& value) {
      return value.first;
    }
    static TValue& GetMappedValue(value_type& value){
      return value.second;
    }
    static const TValue& GetMappedValue(const value_type& value) {
        return value.second;
    }
  };

  //specialization for when value is the key
  template <typename TKey>
  struct DefaultTraits<TKey,void>
  {
    using key_type = TKey;
    using value_type = TKey;
    static const key_type&  Key(const value_type& value) {return value;}
  };


  template<
    typename TKey,
    typename TValue,
    typename TComp = std::less<TKey>,
    typename Traits = DefaultTraits<TKey,TValue>
  >
  class RBTree
  {
  
  protected:  
    using key_type =  Traits::key_type;
    using value_type = Traits::value_type;
    using comparator_type = TComp;

    using reference = value_type&; 
    using const_reference = const value_type&;
    using pointer = value_type*;
    

    enum class Colour : uint16_t {Red, Black};
    
  protected:
    
    struct RBNode
    {
      RBNode* left = nullptr;
      RBNode* right = nullptr;
      RBNode* parent = nullptr;
      Colour colour = Colour::Red;
      value_type value;

      RBNode() = default;

      template<typename... Args>
      RBNode(Args&&... args) : value(std::forward<Args>(args)...) {}
    };
  
  public:  

    class Iterator
    {
    private:
      using iterator_category = std::forward_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      
    public:
      Iterator& Next() {
        if(m_node == m_nil)
          return *this;
        if( m_node->right != m_nil) {  //find min of right tree
          m_node = m_node->right;
          while( (m_node->left != m_nil)  )   
            m_node = m_node->left;
          return *this;  
        }
        while(m_node->parent != m_nil) {
          auto m_node_save = m_node;
          m_node = m_node->parent;
          if(m_node_save->parent->left == m_node_save) //m_node is a left child
            return *this;
        }
        m_node = m_nil;
        return *this;  
      }
      Iterator& Prev()
      {
        if(m_node == m_nil)
          return *this;
        if( m_node->left != m_nil) { //find max of left tree
          m_node = m_node->left;
          while( (m_node->right != m_nil)  )   
            m_node = m_node->right;
          return *this;  
        }
        while(m_node->parent != m_nil) {
          auto m_node_save = m_node;
          if(m_node_save->parent->right == m_node_save) //m_node is a left child
            return *this;
        }
        m_node = m_nil;
        return *this;  
      }
      Iterator& operator++ () { 
        return Next();
      }
      bool operator == (const Iterator& other) const {
        return (m_node == other.m_node) && (m_nil == other.m_nil);
      }
      bool operator != (const Iterator& other) const {
        return !(*this == other);
      }
      reference operator* () const { 
        SPG_ASSERT(m_node != nullptr); 
        return m_node->value;
      }
      pointer operator->() const {
        return &(operator*());
      }
    
    private:
      friend class RBTree;
      template<typename,typename,typename,typename> friend class RBTreeTraversable;
      Iterator() = default;
      Iterator(RBNode* node, RBNode* nil) : m_node{node}, m_nil{nil} {}
      RBNode* m_node;
      RBNode* m_nil;
    };

  public:
    RBTree(TComp comp = TComp());
    RBTree(const std::vector<value_type>& elements, TComp comp = TComp());
    RBTree(const RBTree& other);
    RBTree(RBTree&& other) noexcept;
    RBTree& operator = (const RBTree& other);
    RBTree& operator = (RBTree&& other) noexcept;
    ~RBTree();
  
    bool Insert(const value_type& element);
    bool Erase(const key_type& key);
    void Clear();
    bool Contains(const key_type& key);
    uint32_t Size();
    bool Empty();
    void InOrderTraverse(std::vector<value_type>& values_out); //Flatten() return the vec
    
    Iterator begin();
    Iterator end();
    Iterator Find(const key_type& key);
    Iterator LowerBound(const key_type& key);
    Iterator UpperBound(const key_type& key);

  #ifdef RBTREE_BASE_TRAVERSABLE
    // For tree traversal (optionally put these in subclass RBTreeTraversable)
    Iterator Root();
    Iterator Parent(Iterator itr);
    Iterator LeftChild(Iterator itr);
    Iterator RightChild(Iterator itr);
    bool IsLeaf(Iterator itr);
    bool IsRoot(Iterator itr);
    bool HasLeft(Iterator itr);
    bool HasRight(Iterator itr);
    Iterator FindSplitPos(const key_type& key_low, const key_type& key_high);
  #endif
    
    void Validate();
  
  protected:
    void InitSentinal();
    RBNode* CreateNode(const value_type& element, RBNode* parent);
    void DeleteNode(RBNode* node);
    bool IsLeaf(RBNode* n);
    bool IsLeftChild(RBNode* node);
    bool IsRightChild(RBNode* node);
    uint32_t NumChildren(RBNode* node);
    RBNode* GrandParent(RBNode* node);
    RBNode* Uncle(RBNode* node);
    RBNode* Sibling(RBNode* node);
    RBNode* Min(RBNode* node);
    RBNode* Max(RBNode* node);
    void SpliceOut(RBNode* node); 
    void Replace(RBNode* node, RBNode* replacement);
    void Transplant(RBNode* u, RBNode* v);
    void RotateLeft(RBNode* node);
    void RotateRight(RBNode* node);
    void InOrderTraverse(RBNode* node, std::vector<value_type>& values_out); 

    void EraseWithoutFixup(RBNode* node); 
    void Erase(RBNode* node);
    
    void InsertFixup(RBNode* node);
    void DeleteFixup(RBNode* x);
    void DeleteFixup_0(RBNode* x);
    void DeleteFixup_1(RBNode* x);
    void DeleteFixup_2(RBNode* x);
    void DeleteFixup_3(RBNode* x);
    void DeleteFixup_4(RBNode* x);

    bool Equal(const key_type& k1, const key_type& k2) { return !m_comp(k1,k2) && !m_comp(k2,k1);}
    bool Less(const key_type& k1, const key_type& k2) { return m_comp(k1,k2);}
    key_type Key(const value_type& value) {return Traits::Key(value);}
    

    //The following is for validation / testing only
    void Validate(RBNode* node, uint32_t black_depth, uint32_t depth, 
      std::vector<uint32_t>& black_depths, std::vector<uint32_t>& depths, uint32_t& node_count);

    using BlackHeights = std::unordered_map<RBNode*, std::vector<int>>;
    using PathStack = std::vector<RBNode*>;
    void Validate(RBNode* node, BlackHeights& black_heights, PathStack& path_stack);
  
  protected:
    RBNode* m_root;
    uint32_t m_node_count = 0;
    RBNode* m_nil;
    comparator_type m_comp;
  };


  RBT_TEMPLATE
  RBT_TYPE::RBTree(TComp comp) : m_comp(comp)
  {
    InitSentinal();
  }

  RBT_TEMPLATE
  RBT_TYPE::RBTree(const std::vector<value_type>& elements, TComp comp) : m_comp(comp)
  {
    InitSentinal();
    for(auto& e : elements) 
      Insert(e);
  }

  RBT_TEMPLATE
  RBT_TYPE::RBTree(const RBTree& other)
  {
    if(&other == this)
      return;
    InitSentinal();
    for(auto& element : const_cast<RBTree&>(other)) {
      Insert(element);
    }  
  }

  RBT_TEMPLATE
  RBT_TYPE::RBTree(RBTree&& other) noexcept {
    if(&other == this)
      return;
    m_root = other.m_root;
    m_nil = other.m_nil;
    m_node_count = other.m_node_count;
    other.InitSentinal();  //keep the sentinal node in other
  }

  RBT_TEMPLATE
  RBT_TYPE & RBT_TYPE::operator = (const RBTree& other)
  {
    if(&other != this) {
      Clear();
      for(auto& element : const_cast<RBTree&>(other))
        Insert(element);
    }
    return *this;
  }

  RBT_TEMPLATE
  RBT_TYPE& RBT_TYPE::operator = (RBTree&& other) noexcept 
  {
    if(&other != this) {
      m_root = other.m_root;
      m_nil = other.m_nil;
      m_node_count = other.m_node_count;
      other.InitSentinal();  //keep the sentinal node in other
    }
    return *this;
  }

  RBT_TEMPLATE
  RBT_TYPE::~RBTree()
  {
    Clear();
    delete m_nil;
    m_nil = m_root = nullptr;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Insert(const value_type& element)
  {
    if(m_root == m_nil) {
      m_root = CreateNode(element, m_nil);
      InsertFixup(m_root); //Todo - not necessary InsertFix will just set to black and return - already black
      return true;
    }

    RBNode* cur = m_root;   
    while(true) {
      if(Equal(Key(element),Key(cur->value))) {
        SPG_ERROR("Failed to insert: {}.  Due to {} already inserted", Key(element), Key(cur->value));
        return false;
      }

      if(Less(Key(element),Key(cur->value))) {
        if(cur->left == m_nil) {
          cur->left = CreateNode(element, cur);
          InsertFixup(cur->left);
          return true;
        } else {
          cur = cur->left;
        }
      }
      else {
        if(cur->right ==  m_nil) {
          cur->right = CreateNode(element, cur);
          InsertFixup(cur->right);
          return true;
        } else {
          cur = cur->right;
        }
      }
    }
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Erase(const key_type& key)
  {
    auto itr = Find(key);
    if(itr == end()) {
      SPG_ERROR("Failed to erase: {}.  Not found", key);
      return false;
    }
    Erase(itr.m_node);
    return true;
  }

  RBT_TEMPLATE
  void RBT_TYPE::Clear()
  {
    while(m_node_count > 0)
      EraseWithoutFixup(m_root);
    m_root = m_nil;  
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Contains(const key_type& key)
  {
    auto itr = Find(key);
    return itr.m_node != m_nil;
  }

  RBT_TEMPLATE
  uint32_t RBT_TYPE::Size()
  {
    return m_node_count;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Empty()
  {
    return (m_node_count == 0);
  }

  RBT_TEMPLATE
  void RBT_TYPE::InOrderTraverse(std::vector<value_type>& values_out)
  {
    InOrderTraverse(m_root, values_out);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::begin()
  {
    auto node = Min(m_root);
    return Iterator(node,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::end()
  {
    return Iterator(m_nil,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Find(const key_type& key)
  {
    RBNode* node = m_root;
    while( (node != m_nil) && !Equal(key, Key(node->value)))  {
      if(Less(key,Key(node->value)))
        node = node->left;
      else if (Less(Key(node->value), key))
        node = node->right;  
    }
    Iterator itr(node, m_nil);
    return itr;
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::LowerBound(const key_type& key)
  {
      //Return iterator to the first element in tree which does not strictly go before key (i.e. either the element is equivalent to value or it goes after value). i.e. Comp(element.key, key) = false
    //Where do elements stop being strictly less than key 
    RBNode* node = m_root;
    RBNode* candidate_node = m_nil;
    while(node != m_nil) {
      if(Equal(Key(node->value), key))
        return Iterator(node, m_nil);
      if(!Less(Key(node->value), key)) {
        if( (candidate_node == m_nil) || Less(Key(node->value), Key(candidate_node->value)))
          candidate_node = node;
      }
      if(Less(key, Key(node->value)))
        node = node->left;
      else
        node = node->right;  
    }
    return Iterator(candidate_node, m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::UpperBound(const key_type& key)
  {
    //Return iterator to the first element in tree that goes after key - i.e. Comp(key, element.key) = true, meaning key must come before element
    //Where do elements start being strictly greater than value.
    RBNode* node = m_root;
    RBNode* candidate_node = m_nil;
    while(node != m_nil) {
      if(Less(key, Key(node->value))) { 
        if( (candidate_node == m_nil) || Less(Key(node->value), Key(candidate_node->value)))
          candidate_node = node;
      }
      if(Less(key,Key(node->value)))
        node = node->left;
      else
        node = node->right;  
    }
    return Iterator(candidate_node, m_nil);
  }

#ifdef RBTREE_BASE_TRAVERSABLE
  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Root()
  {
    return Iterator(m_root,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Parent(RBT_TYPE::Iterator itr)
  {
    return Iterator(itr.m_node->parent,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::LeftChild(RBT_TYPE::Iterator itr)
  {
    return Iterator(itr.m_node->left,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::RightChild(RBT_TYPE::Iterator itr)
  {
    return Iterator(itr.m_node->right,m_nil);
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsLeaf(RBT_TYPE::Iterator itr)
  {
    return IsLeaf(itr.m_node);
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsRoot(RBT_TYPE::Iterator itr)
  {
    return itr.m_node == m_root;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::HasLeft(RBT_TYPE::Iterator itr)
  {
    return itr.m_node->left != m_nil;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::HasRight(RBT_TYPE::Iterator itr)
  {
    return itr.m_node->right != m_nil;
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::FindSplitPos(const key_type& key_low, const key_type& key_high)
  {
    //Todo Make sure Less() is consisten for open verses closed intervals
    SPG_ASSERT(Less(key_low,key_high));
    RBNode* node = m_root;
    while( !IsLeaf(node) && (!Less(node->value,key_high) || Less(node->value,key_low)) ) {
      if(!Less(node->value,key_high))
        node = node->left;
      else
        node = node->right;  
    }
    return Iterator(node, m_nil);
  }
#endif

  
  RBT_TEMPLATE
  void RBT_TYPE::Validate()
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

    BlackHeights black_heights;
    PathStack path_stack;
    Validate(m_root, black_heights, path_stack);
    int max_height = -1;
    for(auto& [node, black_heights] : black_heights) {
      if(node != m_nil) {
        auto max = *(std::max_element(black_heights.begin(), black_heights.end()));
        auto min = *(std::min_element(black_heights.begin(), black_heights.end()));
        if(min != max)
          SPG_ERROR("Node: {}: Min/Max black height: {},{}", Key(node->value), min,max);
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
  *************************************************************/
  RBT_TEMPLATE
  void RBT_TYPE::InitSentinal()
  {
    m_nil = m_root = nullptr;
    m_node_count = 0;
    m_nil = new RBNode();
    m_nil->parent = m_nil->left=m_nil->right = m_nil;
    m_nil->colour = RBTree::Colour::Black;
    m_root = m_nil;  
  }
  
  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::CreateNode(const value_type& element, RBNode* parent)
  {
    RBNode* node = new RBNode(element);
    node->parent = parent;
    node->left = node->right = m_nil;
    m_node_count++;
    return node;
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteNode(RBNode* node)
  {
    SPG_ASSERT(node != nullptr);
    SPG_ASSERT(node != m_nil);
    delete node;
    m_node_count--;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsLeaf(RBNode* n) 
  {
    //m_nil will also be a leaf
    return (n->left == m_nil) && (n->right == m_nil); 
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsLeftChild(RBNode* node)
  {
    return ( (node==m_root) ? false : (node->parent->left == node));
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsRightChild(RBNode* node)
  {
    return ( (node==m_root) ? false : (node->parent->right == node));
  }

  RBT_TEMPLATE
  uint32_t RBT_TYPE::NumChildren(RBNode* node)
  {
    uint32_t count = 0;
    if(node->left != m_nil)
      ++count;
    if(node->right != m_nil)
      ++count;  
    return count;  
  }

  RBT_TEMPLATE 
  RBT_TYPE::RBNode* RBT_TYPE::GrandParent(RBNode* node)
  {
    return ((node->parent == m_nil) ? m_nil : node->parent->parent );
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Uncle(RBNode* node)
  {
    RBNode* g = GrandParent(node);
    if( (g != m_nil) && IsLeftChild(node->parent) )
      return g->right;
    if( (g != m_nil) && IsRightChild(node->parent) )
      return g->left;  
    return m_nil;   
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Sibling(RBNode* node)
  {
    if(node == m_root)
      return m_nil;
    return IsLeftChild(node) ? node->parent->right : node->parent->left;
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Min(RBNode* node)
  {
    if(node == m_nil)
      return m_nil;
    while( (node->left != m_nil)  )   
      node = node->left;
    return node;  
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Max(RBNode* node)
  {
    if(node == m_nil)
      return m_nil;
    while( (node->right != m_nil)  )   
      node = node->right;
    return node;  
  }

  RBT_TEMPLATE
  void RBT_TYPE::Transplant(RBNode* u, RBNode* v)
  { //subtree u is replaced with subtree v (v is a descendent node of u).
    if(u == m_root)
      m_root = v;
    else if(IsRightChild(u))  
      u->parent->right = v;
    else
      u->parent->left = v;
    v->parent = u->parent;
  }

  RBT_TEMPLATE
  void RBT_TYPE::SpliceOut(RBNode* node)
  { //Splice out node from the tree (at most 1 non-null child).
    if(node == m_nil)
      return;
    SPG_ASSERT( !((node->left != m_nil) && (node->right != m_nil)) ); 
    if( (node->left == m_nil) && (node->right == m_nil))
      Transplant(node, node->right);   //m_nil->parant = node->parent
    else if(node->right == m_nil) 
      Transplant(node, node->left);   
    else if(node->left == m_nil) 
      Transplant(node,node->right);
  }

  RBT_TEMPLATE
  void RBT_TYPE::Replace(RBNode* node, RBNode* replacement)
  {
    if(node == m_nil)
      return;

    if(replacement == m_nil)  //if replacement is nil, node should have no children
      SPG_ASSERT( (node->left == m_nil) && (node->right == m_nil)); 
    
    Transplant(node,replacement); 
    if(replacement != m_nil) {
      replacement->left = node->left;
      replacement->right = node->right;

      node->left->parent = replacement;
      node->right->parent = replacement;
    }
  } 

  RBT_TEMPLATE
  void RBT_TYPE::RotateLeft(RBNode* node)
  {
    SPG_ASSERT(node != m_nil);
    SPG_ASSERT(node->right != m_nil);
    auto y = node->right; 

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

  RBT_TEMPLATE
  void RBT_TYPE::RotateRight(RBNode* node)
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

  RBT_TEMPLATE
  void RBT_TYPE::InOrderTraverse(RBNode* node, std::vector<value_type>& values_out)
  {
    if(node == m_nil)
      return;
    InOrderTraverse(node->left, values_out);
    values_out.push_back(node->value);
    InOrderTraverse(node->right,values_out);
  }

  RBT_TEMPLATE
  void RBT_TYPE::EraseWithoutFixup(RBNode* node)
  {
    if(node == m_nil)
      return;
    if(NumChildren(node) < 2) 
      SpliceOut(node);
    else {  
      RBNode* successor = Min(node->right);
      SpliceOut(successor);
      Replace(node,successor);
    } 
    DeleteNode(node);
  }

  RBT_TEMPLATE
  void RBT_TYPE::Erase(RBNode* node)
  {
    if(node == m_nil)
      return;
    RBNode* r = m_nil; //replacement
    RBNode* x = m_nil; //replacement's right child if replacement not nil
    
    if( (node->left == m_nil) && (node->right == m_nil) ) {
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
  
  RBT_TEMPLATE
  void RBT_TYPE::InsertFixup(RBNode* node)
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
      else if(IsLeftChild(cur) && IsLeftChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        cur->parent->colour = RBTree::Colour::Black;
        SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
        grandparent->colour = RBTree::Colour::Red;
        RotateRight(grandparent); 
        return; //new grandparent is black - nothing more to fix
      }
      //case 4 (right, right)
      else if(IsRightChild(cur) && IsRightChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        cur->parent->colour = RBTree::Colour::Black;
        SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
        grandparent->colour = RBTree::Colour::Red;
        RotateLeft(grandparent); //new grandparent is black - nothing more to fix
        return;
      }
      //case 5 (left, right)
      else if(IsRightChild(cur) && IsLeftChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        RotateLeft(cur->parent); 
        cur = cur->left; //changes it to case 3 - fix in next iteration
      }
      //case 6 (right, left)
      else if(IsLeftChild(cur) && IsRightChild(cur->parent) && (uncle->colour == RBTree::Colour::Black)) {
        RotateRight(cur->parent); 
        cur = cur->right; //changes it to case 4 - fix in next iteration
      }
      else
        SPG_ASSERT(false);
    }
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup(RBNode* x)
  {
    RBNode* w = Sibling(x);

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

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_0(RBNode* x)
  {
      x->colour = RBTree::Colour::Black;
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_1(RBNode* x)
  {
    RBNode* w = Sibling(x);
    w->colour = Colour::Black;
    x->parent->colour = Colour::Red;
    if(IsLeftChild(x)) 
      RotateLeft(x->parent);
    else 
      RotateRight(x->parent);   
    w = Sibling(x); //not really needed - w will be set from x in the next call to DeleteFixup_*()
    DeleteFixup(x);
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_2(RBNode* x)
  {
    RBNode* w = Sibling(x);
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

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_3(RBNode* x)
  {
    RBNode* w = Sibling(x);
    if(IsLeftChild(x)) 
      w->left->colour = Colour::Black;
    else 
      w->right->colour = Colour::Black;
    w->colour = Colour::Red;
    if(IsLeftChild(x)) 
      RotateRight(w);
    else
      RotateLeft(w);
    w = Sibling(x);  //not really needed - w will be set from x in the next call to DeleteFixup_*()
    DeleteFixup_4(x); 
  }

  RBT_TEMPLATE
  void RBT_TYPE::DeleteFixup_4(RBNode* x)
  {
    RBNode* w = Sibling(x);
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

  RBT_TEMPLATE
  void RBT_TYPE::Validate(RBNode* node, uint32_t black_depth, uint32_t depth, 
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
      SPG_ASSERT(Less(Key(node->left->value),Key(node->value)));
    if(node->right != m_nil)
      SPG_ASSERT(Less(Key(node->value),Key(node->right->value)));  

    node_count++;  
    black_depth += (node->colour == RBTree::Colour::Black) ? 1 : 0;
    
    Validate(node->left, black_depth, depth+1, black_depths, depths, node_count);
    Validate(node->right, black_depth, depth+1, black_depths, depths, node_count);
  }

  RBT_TEMPLATE
  void RBT_TYPE::Validate(RBNode* node, BlackHeights& black_heights, PathStack& path_stack)
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
        RBNode* n = *r_itr;
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

  /*************************************************************
  * Map & Set
  *************************************************************/
  
  //template deduction guide:
  // template<typename TKey>
  // RBTree(DefaultValueTraits<TKey>) -> RBTree<DefaultValueTraits<TKey>>;

 
  template<
    typename TKey, 
    typename TVal, 
    typename TComp = std::less<TKey>,
    typename Traits = DefaultTraits<TKey,TVal>
  >
  using Map = RBTree<TKey,TVal,TComp,Traits>;

  template<
    typename TKey,
    typename TComp = std::less<TKey>,
    typename Traits = DefaultTraits<TKey,void>
  >
  using Set = RBTree<TKey,void,TComp,Traits>;

  //=============================================================

  static void Test_RB_Tree() 
  {
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("Red-Black Tree");
    SPG_WARN("-------------------------------------------------------------------------");
    {

      DefaultTraits<int> traits1;
      DefaultTraits<int,float> traits2;

      std::vector<std::pair<const int,int>> map_vals {{2,200},{11,1100},{4,400},{125,12500},{15,1500},{3,300},{9,900},{32,3200},{71,7100},{43,4300},{27,2700},{1,100}};

      Geom::Map<int, int> my_map(map_vals);
      my_map.Validate();

      //Traversal and iterating
      {
        SPG_WARN("TRAVERSAL AND ITERATING ")
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse");
        my_map.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        
        // SPG_INFO("Ranged for loop traverse");
        // for(auto& [key,val] : my_map) {
        //   SPG_TRACE("[{},{}]", key, val);
        // }

        SPG_INFO("Loop using Iterator")
        for(auto itr = my_map.begin(); itr != my_map.end(); ++itr) {
          auto& [key,val] = *itr;
          SPG_TRACE("[{},{}]", key, val);
        }
      }

      //Find(), Contains()
      {
        SPG_WARN("FIND, CONTAINS ")
        for(auto& [key, value]: map_vals) {
          SPG_INFO("Contains {}? {}", key, my_map.Contains(key));
        }
        SPG_INFO("Contains {}? {}", -15, my_map.Contains(-15));
        SPG_INFO("Contains {}? {}", 0, my_map.Contains(0));
        SPG_INFO("Contains {}? {}", 160, my_map.Contains(160));
        SPG_INFO("Contains {}? {}", 33, my_map.Contains(33));
      }

      {
        SPG_WARN("LOWERBOUND UPPERBOUND ")
        for(auto& [key, value]: map_vals) {
          auto itr = my_map.LowerBound(key);
          if(itr == my_map.end()) {
            SPG_INFO("LowerBound {}? end", key);
          }
          else {
            SPG_INFO("LowerBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_map.LowerBound(k);
          if(itr == my_map.end()) {
            SPG_INFO("LowerBound {}? end", k);
          }
          else {
            SPG_INFO("LowerBound {}? {}", k, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          auto itr = my_map.UpperBound(key);
          if(itr == my_map.end()) {
            SPG_INFO("UpperBound {}? end", key);
          }
          else {
            SPG_INFO("UpperBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_map.UpperBound(k);
          if(itr == my_map.end()) {
            SPG_INFO("UpperBound {}? end", k);
          }
          else {
            SPG_INFO("UpperBound {}? {}", k, (*itr).first);
          }
        }
        
      }

      //Deletion
      {
        SPG_WARN("DELETION ")
        my_map.Erase(1);
        my_map.Erase(71);
        my_map.Erase(27);
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse After erase");
        my_map.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        my_map.Validate();
      }

      //Stress test with large data, insertion, erasure, clear
      {
        SPG_WARN("STRESS TEST WITH LARGE AMOUNT OF DATA ")
        my_map.Clear();
        //Add a bunch of random values
        std::vector<int> rb_values;
        std::vector<bool> rb_values_to_delete;
      
        const uint32_t RB_NUM_VALS = 10000;
        const int RB_MIN_VAL = -100000;
        const int RB_MAX_VAL = 100000;
    
        std::random_device rd;                         
        std::mt19937 mt(rd()); 
        std::uniform_int_distribution<int> dist(RB_MIN_VAL, RB_MAX_VAL); 
      
        for(int i=0; i< RB_NUM_VALS; i++) {
          int val = dist(mt);
          auto element = std::make_pair(val,val*100);
          if(my_map.Insert(element)) {
            rb_values.push_back(val);
            rb_values_to_delete.push_back(false);    
          }
        }
        my_map.Validate();

        //pick values to delete at random
        std::uniform_int_distribution<int> idist(0, rb_values.size()-1);  
        const uint32_t RB_NUM_VALS_TO_DEL = rb_values.size()/2;
        for(int i=0; i<RB_NUM_VALS_TO_DEL; i++ ) {
          int idx = 0;
          do {
            idx = idist(mt);
          } while (rb_values_to_delete[idx] == true);
          rb_values_to_delete[idx] = true;
        }
        
        SPG_INFO("Deleting {} values at random ", RB_NUM_VALS_TO_DEL);
        for(int i=0; i<rb_values_to_delete.size(); i++ ) {
          if(!rb_values_to_delete[i])
            continue;
          my_map.Erase(rb_values[i]);
        }
        my_map.Validate();
        SPG_INFO("Clearing Tree");
        my_map.Clear();
        my_map.Validate();
        SPG_INFO("Tree Size {}: ", my_map.Size());

        SPG_INFO("Add a few more elements");
        for(auto& element : map_vals)
          my_map.Insert(element);
        SPG_INFO("Ranged for loop traverse");
        for(auto& [key,val] : my_map) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("Tree Size {}: ", my_map.Size());
        my_map.Validate();
      }
    
      //Copy move constructor, assignment operator
      {
        auto t2 = my_map; //copy constructor
        Geom::Map<int,int> t3;
        t3 = t2; //copy assignment;
        SPG_INFO("t2:");
        for(auto& [key,val] : t2) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t3:");
        for(auto& [key,val] : t3) {
          SPG_TRACE("[{},{}]", key, val);
        }
        auto t4 = std::move(t2); //move ctr
        Geom::Map<int,int> t5;
        t5 = std::move(t3); //move assignment

        SPG_INFO("After move:");
        SPG_INFO("t2:");
        for(auto& [key,val] : t2) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t3:");
        for(auto& [key,val] : t3) {
          SPG_TRACE("[{},{}]", key, val);
        }

        SPG_INFO("t4:");
        for(auto& [key,val] : t4) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("t5:");
        for(auto& [key,val] : t5) {
          SPG_TRACE("[{},{}]", key, val);
        }
      }

      {
        SPG_WARN("IN-ORDER TRAVERSAL OF BALANCED TREE ")
        std::vector<float> vals{26,32,43,11,15,100,17,7,87,42,150,111, 27, 54,1,33,200,88,99,0};
        std::vector<float> ordered_vals_out;
        Geom::Set<float> rb_tree;
        for(auto v : vals)
          rb_tree.Insert(v);
        rb_tree.InOrderTraverse(ordered_vals_out);
        SPG_WARN("Elements in RBTree {}", rb_tree.Size())
        for(auto& v : ordered_vals_out) {
          SPG_TRACE(v);
        }

      }
    }

 }
#endif

} //namespace Geom