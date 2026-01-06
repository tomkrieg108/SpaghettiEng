#pragma once
#include <Common/Common.h>
#include <random>
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
  #define RBTREE_VERS 1
  

#if (RBTREE_VERS==1)
  
  //#define RBTREE_BASE_TRAVERSABLE
  #define RBT_TEMPLATE template<typename TKey,typename TValue,typename TComp,typename Traits>
  #define RBT_TYPE RBTree<TKey,TValue,TComp,Traits>

  // forward declaration — definition comes below Enables DefaultValueTraits<int>; instead of DefaultValueTraits<int,void>. 
  //todo: Except that is doesn't seem to work!
  template<typename TKey, typename TValue = void>
  struct DefaultValueTraits;   

  template <typename TKey, typename TValue>
  struct DefaultValueTraits
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
  struct DefaultValueTraits<TKey,void>
  {
    using key_type = TKey;
    using value_type = TKey;
    static const key_type&  Key(const value_type& value) {return value;}
  };

  template<
    typename TKey,
    typename TValue,
    typename TComp = std::less<TKey>,
    typename Traits = DefaultValueTraits<TKey,TValue>
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
      using iterator_category = std::bidirectional_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      
    public:
      Iterator() = default;
      Iterator& Next()  {
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
          m_node = m_node->parent;
          if(m_node_save->parent->right == m_node_save) //m_node is a right child
            return *this;
        }
        m_node = m_nil;
        return *this;  
      }
      Iterator& operator++ () { 
        return Next();
      }
      Iterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
      }
      Iterator& operator--() {
        return Prev();
      }
      Iterator operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
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
      RBNode* Get() const { return m_node;}
    
    private:
      friend class RBTree;
      template<typename,typename,typename,typename> friend class RBTreeTraversable;
      Iterator(RBNode* node, RBNode* nil) : m_node{node}, m_nil{nil} {}
      RBNode* m_node = nullptr;
      RBNode* m_nil = nullptr;
    };

  public:
    RBTree(TComp comp = TComp());
    explicit RBTree(const std::vector<value_type>& elements, TComp comp = TComp());
    
    RBTree(const RBTree& other);
    RBTree(RBTree&& other) noexcept;
    RBTree& operator = (const RBTree& other);
    RBTree& operator = (RBTree&& other) noexcept;
    ~RBTree();
  
    Iterator Insert(const value_type& element);
    //Iterator Insert(Iterator pos, const value_type& element); //Todo - Insert position specified by iterator

    bool Erase(const key_type& key);
    void Erase(Iterator itr) {this->Erase(itr.m_node);}
    // Iterator Erase(const key_type& key); //Todo - update to return iterator
    // Iterator Erase(Iterator pos);

    void Clear();
    bool Contains(const key_type& key) const;
    uint32_t Size() const;
    bool Empty() const;
    void InOrderTraverse(std::vector<value_type>& values_out) const; //Flatten() return the vec
    
    Iterator begin() const;
    Iterator end() const;
    Iterator Find(const key_type& key) const;
    Iterator LowerBound(const key_type& key) const;
    Iterator UpperBound(const key_type& key) const;

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
    
    void Validate() const;
  
  protected:
    void InitSentinal();
    RBNode* CreateNode(const value_type& element, RBNode* parent);
    void DestroyNode(RBNode* node);
    bool IsLeaf(RBNode* n) const;
    bool IsLeftChild(RBNode* node) const;
    bool IsRightChild(RBNode* node) const;
    uint32_t NumChildren(RBNode* node) const;
    RBNode* GrandParent(RBNode* node) const;
    RBNode* Uncle(RBNode* node) const;
    RBNode* Sibling(RBNode* node) const;
    RBNode* Min(RBNode* node) const;
    RBNode* Max(RBNode* node) const;
    void InOrderTraverse(RBNode* node, std::vector<value_type>& values_out) const; 
    void SpliceOut(RBNode* node); 
    void Replace(RBNode* node, RBNode* replacement);
    void Transplant(RBNode* u, RBNode* v);
    void RotateLeft(RBNode* node);
    void RotateRight(RBNode* node);
    
    Iterator Insert(RBNode* node);
    void EraseWithoutFixup(RBNode* node); 
    void Erase(RBNode* node);
    
    void InsertFixup(RBNode* node);
    void DeleteFixup(RBNode* x);
    void DeleteFixup_0(RBNode* x);
    void DeleteFixup_1(RBNode* x);
    void DeleteFixup_2(RBNode* x);
    void DeleteFixup_3(RBNode* x);
    void DeleteFixup_4(RBNode* x);

    bool Equal(const key_type& k1, const key_type& k2) const { return !m_comp(k1,k2) && !m_comp(k2,k1);}
    bool Less(const key_type& k1, const key_type& k2) const { return m_comp(k1,k2);}
    
    key_type Key(const value_type& value) const {return Traits::Key(value);}

   
    //The following is for validation / testing only
    void Validate(RBNode* node, uint32_t black_depth, uint32_t depth, 
      std::vector<uint32_t>& black_depths, std::vector<uint32_t>& depths, uint32_t& node_count) const;

    using BlackHeights = std::unordered_map<RBNode*, std::vector<int>>;
    using PathStack = std::vector<RBNode*>;
    void Validate(RBNode* node, BlackHeights& black_heights, PathStack& path_stack) const;
  
  protected:
    RBNode* m_root = nullptr;
    RBNode* m_nil = nullptr;
    uint32_t m_node_count = 0;
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

#if 0  
  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Insert(const value_type& element)
  {
    auto node = CreateNode(element, m_nil);
    auto itr = Insert(node);
    if(itr != end()) 
       m_node_count++;
    else
      DestroyNode(node);
    return itr;
  }
#endif  

#if 1  
  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Insert(const value_type& element)
  {
    if(m_root == m_nil) {
      m_root = CreateNode(element, m_nil);
      InsertFixup(m_root); 
      return Iterator(m_root,m_nil);
    }

    RBNode* cur = m_root;   
    while(true) {
      if(Equal(Key(element),Key(cur->value))) {
        SPG_ERROR("Failed to insert: {}.  Due to {} already inserted", Key(element), Key(cur->value));
        return end();
      }

      if(Less(Key(element),Key(cur->value))) {
        if(cur->left == m_nil) {
          cur->left = CreateNode(element, cur);
          InsertFixup(cur->left);
          return Iterator(cur->left,m_nil);
        } else {
          cur = cur->left;
        }
      }
      else {
        if(cur->right ==  m_nil) {
          cur->right = CreateNode(element, cur);
          InsertFixup(cur->right);
          return Iterator(cur->right,m_nil);
        } else {
          cur = cur->right;
        }
      }
    }
  }
#endif

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
  bool RBT_TYPE::Contains(const key_type& key) const
  {
    auto itr = Find(key);
    return itr.m_node != m_nil;
  }

  RBT_TEMPLATE
  uint32_t RBT_TYPE::Size() const
  {
    return m_node_count;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::Empty() const
  {
    return (m_node_count == 0);
  }

  RBT_TEMPLATE
  void RBT_TYPE::InOrderTraverse(std::vector<value_type>& values_out) const
  {
    InOrderTraverse(m_root, values_out);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::begin() const
  {
    auto node = Min(m_root);
    return Iterator(node,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::end() const
  {
    return Iterator(m_nil,m_nil);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Find(const key_type& key) const
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
  RBT_TYPE::Iterator RBT_TYPE::LowerBound(const key_type& key) const
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
  RBT_TYPE::Iterator RBT_TYPE::UpperBound(const key_type& key) const
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
  void RBT_TYPE::Validate() const
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
    int max_height = 0;
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
  void RBT_TYPE::DestroyNode(RBNode* node)
  {
    SPG_ASSERT(node != nullptr);
    SPG_ASSERT(node != m_nil);
    m_node_count--;
    delete node;
    node = nullptr;
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsLeaf(RBNode* n) const
  {
    //m_nil will also be a leaf
    return (n->left == m_nil) && (n->right == m_nil); 
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsLeftChild(RBNode* node) const
  {
    return ( (node==m_root) ? false : (node->parent->left == node));
  }

  RBT_TEMPLATE
  bool RBT_TYPE::IsRightChild(RBNode* node) const
  {
    return ( (node==m_root) ? false : (node->parent->right == node));
  }

  RBT_TEMPLATE
  uint32_t RBT_TYPE::NumChildren(RBNode* node) const
  {
    uint32_t count = 0;
    if(node->left != m_nil)
      ++count;
    if(node->right != m_nil)
      ++count;  
    return count;  
  }

  RBT_TEMPLATE 
  RBT_TYPE::RBNode* RBT_TYPE::GrandParent(RBNode* node) const
  {
    return ((node->parent == m_nil) ? m_nil : node->parent->parent );
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Uncle(RBNode* node) const
  {
    RBNode* g = GrandParent(node);
    if( (g != m_nil) && IsLeftChild(node->parent) )
      return g->right;
    if( (g != m_nil) && IsRightChild(node->parent) )
      return g->left;  
    return m_nil;   
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Sibling(RBNode* node) const
  {
    if(node == m_root)
      return m_nil;
    return IsLeftChild(node) ? node->parent->right : node->parent->left;
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Min(RBNode* node) const
  {
    if(node == m_nil)
      return m_nil;
    while( (node->left != m_nil)  )   
      node = node->left;
    return node;  
  }

  RBT_TEMPLATE
  RBT_TYPE::RBNode* RBT_TYPE::Max(RBNode* node) const
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
  void RBT_TYPE::InOrderTraverse(RBNode* node, std::vector<value_type>& values_out) const
  {
    if(node == m_nil)
      return;
    InOrderTraverse(node->left, values_out);
    values_out.push_back(node->value);
    InOrderTraverse(node->right,values_out);
  }

  RBT_TEMPLATE
  RBT_TYPE::Iterator RBT_TYPE::Insert(RBNode* node)
  {
    SPG_ASSERT(node != nullptr)
    if(node == m_nil)
      return Iterator(m_nil,m_nil);

    if(m_root == m_nil) {
      node->parent = m_nil;
      m_root = node;
      InsertFixup(m_root); 
      return Iterator(m_root,m_nil);
    }

    RBNode* cur = m_root;   
    while(true) {
      if(Equal(Key(node->value),Key(cur->value))) {
        SPG_ERROR("Failed to insert: {}.  Due to {} already inserted", Key(node->value), Key(cur->value));
        return end();
      }

      if(Less(Key(node->value),Key(cur->value))) {
        if(cur->left == m_nil) {
          node->parent = cur;
          cur->left = node;
          InsertFixup(cur->left);
          return Iterator(cur->left,m_nil);
        } else {
          cur = cur->left;
        }
      }
      else {
        if(cur->right ==  m_nil) {
          node->parent = cur;
          cur->right = node;
          InsertFixup(cur->right);
          return Iterator(cur->right,m_nil);
        } else {
          cur = cur->right;
        }
      }
    }   
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
    DestroyNode(node);
    //m_node_count--;
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
      //nothing to do!
    }
    else if( (node->colour == Colour::Red) && (r != m_nil) && (r->colour == Colour::Black) ) {
      r->colour = Colour::Red;
      DeleteFixup(x);
    }
    else if( (node->colour == Colour::Black) && (r->colour == Colour::Red) ) {
      r->colour = Colour::Black;
    }
    else if( (x==m_root) && (node->colour == Colour::Black) && (r->colour == Colour::Black) ) {
      // Nothing to do (This happens when the last element in the tree is deleted)
    }
    else if((x!=m_root) && (node->colour == Colour::Black) &&  (r->colour == Colour::Black)) {
      DeleteFixup(x);
    }
    else
      SPG_ASSERT(false);

    DestroyNode(node);
    //m_node_count--;  
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
      std::vector<uint32_t>& black_depths, std::vector<uint32_t>& depths, uint32_t& node_count) const
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
  void RBT_TYPE::Validate(RBNode* node, BlackHeights& black_heights, PathStack& path_stack) const
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
* Iterator free functions
*************************************************************/

template <typename Iterator>
Iterator Next(Iterator itr, std::ptrdiff_t n = 1 )
{
  while (n-- >0) ++itr;
  return itr;
}

template <typename Iterator>
Iterator Prev(Iterator itr, std::ptrdiff_t n = 1 )
{
  while (n-- >0) --itr;
  return itr;
}

template <typename Iterator>
void Advance(Iterator& itr, std::ptrdiff_t n )
{
  if (n >= 0) {
        while (n--) ++itr;
  } else {
      while (n++) --itr;
  }
}

template <typename Iterator>
std::ptrdiff_t Distance(Iterator first, Iterator last )
{
  std::ptrdiff_t n = 0;
  while (first != last) { ++first; ++n; }
  return n;
}

template <typename Iterator>
auto GetPointer(Iterator itr)
{
  return itr.Get();
}

  /*************************************************************
  * TESTING
  *************************************************************/

  void Test_RBTree();
  
 #endif

  namespace RBTree_V2 
  {
  
    template<typename TNode, typename TValue>
    struct RBNodeBase
    {
      enum class Colour : uint16_t { Red, Black };

      TNode* left = nullptr;
      TNode* right = nullptr;
      TNode* parent = nullptr;

      Colour colour = Colour::Red;
      TValue value;

      RBNodeBase() = default;

      template<typename... Args>
      RBNodeBase(Args&&... args): value(std::forward<Args>(args)...) {}
    };

    template<typename TValue>
    struct DefaultRBNode : RBNodeBase<DefaultRBNode<TValue>,TValue>
    {
      using Base = RBNodeBase<DefaultRBNode<TValue>, TValue>;
      using Base::Base;
    };

    template<
      typename TValue, 
      typename TComp = std::less<TValue>,
      typename TNode = void
    >
    class RBTree
    {

// *Public member types
#if 1      
    public:

      // Member types (mimicking STL!)
      using key_type =  TValue;
      using value_type = TValue;
      using reference = value_type&; 
      using pointer = value_type*;
      // not currently used:
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using key_compare = TComp;
      using value_compare = TComp;
      using const_reference = const value_type&;
      using const_pointer = const pointer;

      using node_type = std::conditional_t<std::is_void_v<TNode>, DefaultRBNode<value_type>,TNode>;

#endif        
      
// *Protected member types
#if 1
    protected:
      using Colour = typename node_type::Base::Colour;
#endif

// *Public Nested classes/structs (Iterator)
#if 1
    public:

      class Iterator
      {
      private:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        
      public:
        Iterator() = default;

        Iterator& operator++()  {
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

        Iterator& operator--() {
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
            m_node = m_node->parent;
            if(m_node_save->parent->right == m_node_save) //m_node is a right child
              return *this;
          }
          m_node = m_nil;
          return *this;  
        }
        
        Iterator operator++(int) {
          auto tmp = *this;
          ++(*this);
          return tmp;
        }

        Iterator operator--(int) {
          auto tmp = *this;
          --(*this);
          return tmp;
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

        node_type* Get() const { return m_node;}
      
      private:
        friend class RBTree;
        Iterator(node_type* node, node_type* nil) : m_node{node}, m_nil{nil} {}
        node_type* m_node = nullptr;
        node_type* m_nil = nullptr;
      }; 

#endif

// *Public functions
#if 1
    public:

      RBTree(TComp comp = TComp()):m_comp{comp} {
          Initialise();
      }

      RBTree(const std::vector<value_type>& elements, TComp comp = TComp()) : RBTree{comp} {
        for(auto& e : elements) 
          Insert(e);
      }

      RBTree(const RBTree& other) {
        if(&other == this)
          return;
        Initialise();
        for(auto& element : const_cast<RBTree&>(other)) {
          Insert(element);
        }  
      }

      RBTree(RBTree&& other) noexcept {
        if(&other == this)
          return;
        m_root = other.m_root;
        m_nil = other.m_nil;
        m_node_count = other.m_node_count;
        other.Initialise(); //Destructor crashes withouth this!
      }

      RBTree& operator = (const RBTree& other) {
        if(&other != this) {
          Clear();
          for(auto& element : const_cast<RBTree&>(other))
            Insert(element);
        }
        return *this;
      }

      RBTree& operator = (RBTree&& other) noexcept {
        if(&other != this) {
          m_root = other.m_root;
          m_nil = other.m_nil;
          m_node_count = other.m_node_count;
          other.Initialise(); //Destructor crashes withouth this!
        }
        return *this;
      }

      ~RBTree() {
        Clear();
        delete m_nil;
        m_nil = m_root = nullptr;
      }

      Iterator Insert(const value_type& element) {
        if(m_root == m_nil) {
          m_root = MakeNode(element, m_nil);
          InsertFixup(m_root); 
          m_node_count++;
          return Iterator(m_root,m_nil);
        }

        node_type* cur = m_root;   
        while(true) {
          if(Equal(Key(element),Key(cur->value))) {
            return end();
          }

          if(Less(Key(element),Key(cur->value))) {
            if(cur->left == m_nil) {
              cur->left = MakeNode(element, cur);
              InsertFixup(cur->left);
              m_node_count++;
              return Iterator(cur->left,m_nil);
            } else {
              cur = cur->left;
            }
          }
          else {
            if(cur->right ==  m_nil) {
              cur->right = MakeNode(element, cur);
              InsertFixup(cur->right);
              m_node_count++;
              return Iterator(cur->right,m_nil);
            } else {
              cur = cur->right;
            }
          }
        }
      }

      Iterator Insert(node_type* node) {
        SPG_ASSERT(node != nullptr)
        if(m_root == m_nil) {
          m_root = node;
          m_root->parent = m_nil;

          InsertFixup(m_root); 
          m_node_count++;
          return Iterator(m_root,m_nil);
        }

        node_type* cur = m_root;   
        while(true) {
          if(Equal(Key(node->value),Key(cur->value))) {
            return end();
          }

          if(Less(Key(node->value),Key(cur->value))) {
            if(cur->left == m_nil) {
              cur->left = node;
              node->parent = cur;

              InsertFixup(cur->left);
              m_node_count++;
              return Iterator(cur->left,m_nil);
            } else {
              cur = cur->left;
            }
          }
          else {
            if(cur->right ==  m_nil) {
              cur->right = node;
              node->parent = cur;

              InsertFixup(cur->right);
              m_node_count++;
              return Iterator(cur->right,m_nil);
            } else {
              cur = cur->right;
            }
          }
        }
      }
    
      bool Erase(const key_type& key) {
        auto itr = Find(key);
        if(itr == end()) {
          SPG_ERROR("Failed to erase: {}.  Not found", key);
          return false;
        }
        Erase(itr.m_node);
        return true;
      }

      void Erase(Iterator itr) {
        Erase(itr.m_node);
      }

      void Clear() {
        while(m_node_count > 0)
          EraseWithoutFixup(m_root);
        m_root = m_nil;  
      }

      bool Contains(const key_type& key) const {
        auto itr = Find(key);
        return itr.m_node != m_nil;
      }

      std::size_t Size() const {
        return m_node_count;
      }

      bool Empty() const {
        return (m_node_count == 0);
      }

      void InOrderTraverse(std::vector<value_type>& values_out) const {
        InOrderTraverse(m_root, values_out);
      }

      Iterator begin() const {
        auto node = Min(m_root);
        return Iterator(node,m_nil);
      }

      Iterator end() const {
        return Iterator(m_nil,m_nil);
      }

      Iterator Find(const key_type& key) const {
        node_type* node = m_root;
        while( (node != m_nil) && !Equal(key, Key(node->value)))  {
          if(Less(key,Key(node->value)))
            node = node->left;
          else if (Less(Key(node->value), key))
            node = node->right;  
        }
        Iterator itr(node, m_nil);
        return itr;
      }

      Iterator LowerBound(const key_type& key) const {
          //Return iterator to the first element in tree which does not strictly go before key (i.e. either the element is equivalent to value or it goes after value). i.e. Comp(element.key, key) = false
        //Where do elements stop being strictly less than key 
        node_type* node = m_root;
        node_type* candidate_node = m_nil;
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

      Iterator UpperBound(const key_type& key) const
      {
        //Return iterator to the first element in tree that goes after key - i.e. Comp(key, element.key) = true, meaning key must come before element
        //Where do elements start being strictly greater than value.
        node_type* node = m_root;
        node_type* candidate_node = m_nil;
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

#endif
      
// *Protected data
#if 1
    protected:
      node_type* m_root = nullptr;
      node_type* m_nil = nullptr;
      std::size_t m_node_count = 0;
      TComp m_comp;  
#endif
      
// *Protected functions
#if 1      
    protected:

      void Initialise() {
        m_nil = AllocateNode();
        m_nil->parent = m_nil->left=m_nil->right = m_nil;
        m_nil->colour = Colour::Black;
        m_node_count = 0;
        m_root = m_nil;  
      }

      node_type* AllocateNode() {
        node_type* node = new node_type();
        SPG_ASSERT(node != nullptr);
        return node;
      }

      node_type* AllocateNode(value_type const& v) {
        node_type* node = new node_type(v);
        SPG_ASSERT(node != nullptr);
        return node;
      }

      void DeallocateNode(node_type* node) {
        delete node;
        node = nullptr;
      }

      node_type* MakeNode(value_type const& v, node_type* parent) {
        node_type* node = AllocateNode(v);
        node->parent = parent;
        node->left = node->right = m_nil;
        node->colour = Colour::Red;
        return node;
      }

      bool Equal(const key_type& k1, const key_type& k2) const { 
        return !m_comp(k1,k2) && !m_comp(k2,k1);
      }
      
      bool Less(const key_type& k1, const key_type& k2) const { 
        return m_comp(k1,k2);
      }
  
      const key_type& Key(const value_type& value) const {
        return value; // key is the value 
      }

      bool IsLeaf(node_type* n) const {
        return (n->left == m_nil) && (n->right == m_nil); 
      }
  
      bool IsLeftChild(node_type* node) const {
        return ( (node==m_root) ? false : (node->parent->left == node));
      }

      bool IsRightChild(node_type* node) const {
        return ( (node==m_root) ? false : (node->parent->right == node));
      }

      std::size_t NumChildren(node_type* node) const {
        std::size_t count = 0;
        if(node->left != m_nil)
          ++count;
        if(node->right != m_nil)
          ++count;  
        return count;  
      }

      node_type* GrandParent(node_type* node) const {
        return ((node->parent == m_nil) ? m_nil : node->parent->parent );
      }

      node_type* Uncle(node_type* node) const {
        node_type* g = GrandParent(node);
        if( (g != m_nil) && IsLeftChild(node->parent) )
          return g->right;
        if( (g != m_nil) && IsRightChild(node->parent) )
          return g->left;  
        return m_nil;   
      }

      node_type* Sibling(node_type* node) const {
        if(node == m_root)
          return m_nil;
        return IsLeftChild(node) ? node->parent->right : node->parent->left;
      }

      node_type* Min(node_type* node) const {
        if(node == m_nil)
          return m_nil;
        while( (node->left != m_nil)  )   
          node = node->left;
        return node;  
      }

      node_type* Max(node_type* node) const {
        if(node == m_nil)
          return m_nil;
        while( (node->right != m_nil)  )   
          node = node->right;
        return node;  
      }

      void Transplant(node_type* u, node_type* v) { 
        //subtree u is replaced with subtree v (v is a descendent node of u).
        if(u == m_root)
          m_root = v;
        else if(IsRightChild(u))  
          u->parent->right = v;
        else
          u->parent->left = v;
        v->parent = u->parent;
      }

      void SpliceOut(node_type* node) { 
        //Splice out node from the tree (at most 1 non-null child).
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

      void Replace(node_type* node, node_type* replacement) {
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

      void RotateLeft(node_type* node) {
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

      void RotateRight(node_type* node) {
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

      void InOrderTraverse(node_type* node, std::vector<value_type>& values_out) const {
        if(node == m_nil)
          return;
        InOrderTraverse(node->left, values_out);
        values_out.push_back(node->value);
        InOrderTraverse(node->right,values_out);
      }

      void EraseWithoutFixup(node_type* node) {
        SPG_ASSERT(node != nullptr);
        if(node == m_nil)
          return;
        if(NumChildren(node) < 2) 
          SpliceOut(node);
        else {  
          node_type* successor = Min(node->right);
          SpliceOut(successor);
          Replace(node,successor);
        } 
        DeallocateNode(node);
        m_node_count--;
      }

      void Erase(node_type* node) {
        SPG_ASSERT(node != nullptr);
        if(node == m_nil)
          return;
        node_type* r = m_nil; //replacement
        node_type* x = m_nil; //replacement's right child if replacement not nil
        
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
          //nothing to do!
        }
        else if( (node->colour == Colour::Red) && (r != m_nil) && (r->colour == Colour::Black) ) {
          r->colour = Colour::Red;
          DeleteFixup(x);
        }
        else if( (node->colour == Colour::Black) && (r->colour == Colour::Red) ) {
          r->colour = Colour::Black;
        }
        else if( (x==m_root) && (node->colour == Colour::Black) && (r->colour == Colour::Black) ) {
          // Nothing to do (This happens when the last element in the tree is deleted)
        }
        else if((x!=m_root) && (node->colour == Colour::Black) &&  (r->colour == Colour::Black)) {
          DeleteFixup(x);
        }
        else
          SPG_ASSERT(false);

        DeallocateNode(node);
        m_node_count--;
        m_nil->parent = m_nil; //reset parent of m_nil to itself
      }
      
      void InsertFixup(node_type* node) {
        if(node ==  m_nil)
          return;
  
        auto cur = node;  
        while(true) {
          if(cur == m_root) {
            cur->colour = Colour::Black;
            return;
          }

          if(cur->parent->colour == Colour::Black)
            return; //nothing more to fix

          auto uncle = Uncle(cur);
          auto grandparent = GrandParent(cur);
          //case 1
          if(IsLeftChild(cur->parent) && (uncle != m_nil) && (uncle->colour == Colour::Red) )  {
            cur->parent->colour = Colour::Black;
            uncle->colour = Colour::Black;
            grandparent->colour = Colour::Red;
            cur = grandparent; //new grandparent is red and may need fixing - continue up tree
          }
          //case 2 (same steps as case 1 to fix)
          else if(IsRightChild(cur->parent) && (uncle != m_nil) && (uncle->colour == Colour::Red) ) {
            cur->parent->colour = Colour::Black;
            uncle->colour = Colour::Black;
            grandparent->colour = Colour::Red;
            cur = grandparent; //new grandparent is red and may need fixing - continue up tree
          }
          //case 3 (left,left)
          else if(IsLeftChild(cur) && IsLeftChild(cur->parent) && (uncle->colour == Colour::Black)) {
            cur->parent->colour = Colour::Black;
            SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
            grandparent->colour = Colour::Red;
            RotateRight(grandparent); 
            return; //new grandparent is black - nothing more to fix
          }
          //case 4 (right, right)
          else if(IsRightChild(cur) && IsRightChild(cur->parent) && (uncle->colour == Colour::Black)) {
            cur->parent->colour = Colour::Black;
            SPG_ASSERT(grandparent != m_nil); //should't occur.  Parent is red, which means it can't be the root (which is black)
            grandparent->colour = Colour::Red;
            RotateLeft(grandparent); //new grandparent is black - nothing more to fix
            return;
          }
          //case 5 (left, right)
          else if(IsRightChild(cur) && IsLeftChild(cur->parent) && (uncle->colour == Colour::Black)) {
            RotateLeft(cur->parent); 
            cur = cur->left; //changes it to case 3 - fix in next iteration
          }
          //case 6 (right, left)
          else if(IsLeftChild(cur) && IsRightChild(cur->parent) && (uncle->colour == Colour::Black)) {
            RotateRight(cur->parent); 
            cur = cur->right; //changes it to case 4 - fix in next iteration
          }
          else
            SPG_ASSERT(false);
        }
      }

      void DeleteFixup(node_type* x) {
        node_type* w = Sibling(x);

        if(x->colour == Colour::Red) {
          DeleteFixup_0(x);
        }

        else if( (x->colour == Colour::Black) && (w->colour == Colour::Red) ) {
          DeleteFixup_1(x);
        }

        else if( (x->colour == Colour::Black) && (w->colour == Colour::Black) ) {

          if ( (w->left->colour == Colour::Black) && (w->right->colour == Colour::Black) ) {
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

      void DeleteFixup_0(node_type* x) {
          x->colour = Colour::Black;
      }

      void DeleteFixup_1(node_type* x) {
        node_type* w = Sibling(x);
        w->colour = Colour::Black;
        x->parent->colour = Colour::Red;
        if(IsLeftChild(x)) 
          RotateLeft(x->parent);
        else 
          RotateRight(x->parent);   
        w = Sibling(x); //not really needed - w will be set from x in the next call to DeleteFixup_*()
        DeleteFixup(x);
      }

      void DeleteFixup_2(node_type* x) {
        node_type* w = Sibling(x);
        w->colour = Colour::Red;
        x = x->parent;  
        if(x->colour == Colour::Red)
          DeleteFixup_0(x);
        else {
          if(x == m_root)
            return;
          DeleteFixup(x);
        }
      }

      void DeleteFixup_3(node_type* x) {
        node_type* w = Sibling(x);
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

      void DeleteFixup_4(node_type* x) {
        node_type* w = Sibling(x);
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

#endif      
        
// *Validation functions        
#if 1
    
    public:

  void Validate() const
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
    int max_height = 0;
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

    protected:
      using BlackHeights = std::unordered_map<node_type*, std::vector<int>>;
      using PathStack = std::vector<node_type*>;
    
      void Validate(node_type* node, uint32_t black_depth, uint32_t depth, 
          std::vector<uint32_t>& black_depths, std::vector<uint32_t>& depths, uint32_t& node_count) const
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
        if(node->left != m_nil) {
          SPG_ASSERT(Less(Key(node->left->value),Key(node->value)));
        }
        if(node->right != m_nil) {
          SPG_ASSERT(Less(Key(node->value),Key(node->right->value)));  
        }
  
        node_count++;  
        black_depth += (node->colour == RBTree::Colour::Black) ? 1 : 0;
        
        Validate(node->left, black_depth, depth+1, black_depths, depths, node_count);
        Validate(node->right, black_depth, depth+1, black_depths, depths, node_count);
      }

      void Validate(node_type* node, BlackHeights& black_heights, PathStack& path_stack) const
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
            node_type* n = *r_itr;
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

#endif

    }; //Class RBTree

// *Free functions for Iterator
#if 1

    template <typename T_Itr>
    T_Itr Next(T_Itr itr, std::ptrdiff_t n = 1 ) {
      while (n-- >0) ++itr;
      return itr;
    }

    template <typename T_Itr>
    T_Itr Prev(T_Itr itr, std::ptrdiff_t n = 1 ) {
      while (n-- >0) --itr;
      return itr;
    }

    template <typename T_Itr>
    void Advance(T_Itr& itr, std::ptrdiff_t n ) {
      if (n >= 0) {
            while (n--) ++itr;
      } else {
          while (n++) --itr;
      }
    }

    template <typename T_Itr>
    std::ptrdiff_t Distance(T_Itr first, T_Itr last ) {
      std::ptrdiff_t n = 0;
      while (first != last) { ++first; ++n; }
      return n;
    }

    template <typename T_Itr>
    auto GetPointer(T_Itr itr) {
      return itr.Get();
    }    

#endif

  
  // * Definition in RBTree.cpp
  void Test_RBTree();

  } //namespace RBTree_V2 

 

} //namespace Geom