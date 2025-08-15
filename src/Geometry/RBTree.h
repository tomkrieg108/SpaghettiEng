#pragma once
#include <Common/Common.h>
#include <functional> //std::less


namespace Geom
{
  inline namespace rbtree_v1
  {
  class RBTree 
  {
  private:

    enum class Colour : uint16_t {Red, Black};
   
    //note: can alternatively declare RBTNode, Iterator here and define them outside

    /*
    alignas(x) means a struct has to start at an address that's an even multiple of X
    Can use alignas(X) for individual members too - says that the start of the member must be at an
    offset that is an even multiple of X.  Better to use defaults for members
    */
    struct /*alignas(8)*/ RBTNode
    {
      RBTNode* left = nullptr;
      RBTNode* right = nullptr;
      RBTNode* parent = nullptr;
      float value = 0;
      Colour colour = Colour::Red;
      RBTNode() = default;
      RBTNode(float val) : value{val} {
      //  constexpr int s = sizeof(Colour); //4
      //  constexpr int s2 = sizeof(RBTNode); //32
      //  constexpr int a = alignof(RBTNode); //8 by default (i.e. the sixe of the largest member)
      }
    };

    class Iterator
    {
      friend class RBTree;
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type        = float;
      using difference_type   = std::ptrdiff_t;
      using pointer           = float*;
      using reference         = float&;
 
      Iterator(RBTree& tree) : m_tree{tree} {}
      Iterator(RBTree& tree, RBTNode* node) : m_tree{tree}, m_node{node} {}
      Iterator(const Iterator& other) = default; //Need this - deleted otherwise
      Iterator& operator = (const Iterator& other) = default; //Need this - deleted otherwise
      Iterator& operator = (Iterator&& other) noexcept { //Need this - deleted otherwise
        if(this != &other) {
          m_tree = other.m_tree;
          m_node = other.m_node;
        }
        return *this;
      }
      Iterator& operator++ () { 
        m_node = m_tree.Next(m_node);
        return *this;
      }

      //Todo - implement this or remove.  Parameter should be int, not int32_t
      //Iterator operator++ (int32_t); //need this for prefix increment apparently

      //Todo - remove this - have a reverse iterator
      Iterator& operator-- () {
        m_node = m_tree.Previous(m_node);
        return *this;
      }
      bool operator == (const Iterator& other) {
        auto tree_addr = std::addressof(m_tree);
        auto tree_addr_other = std::addressof(other.m_tree);
        return (tree_addr == tree_addr_other) && (m_node == other.m_node);
      }
      bool operator != (const Iterator& other) {
        return !(*this == other);
      }
      const reference operator* () { 
        SPG_ASSERT(m_node != nullptr); 
        return m_node->value; 
      }
      //Optional - remove for now - return value should prevent modification of the stored value
      // pointer operator->() {
      //   SPG_ASSERT(m_node != nullptr); 
      //   return &(m_node->value);
      // }

    private:
      RBTree& m_tree; //Todo - STL iterators don't store a ref to the continer.  Nav logic is embedded in iterator itself
      RBTNode* m_node = nullptr;
    };

    //Todo - ReverseIterator

  public:

    RBTree();
    RBTree(const std::vector<float>& values);
    ~RBTree();
    //Todo - implement these:
    // RBTree(const RBTree& other);
    // RBTree(RBTree&& other) noexcept;
    // RBTree& operator = (const RBTree& other);
    // RBTree& operator = (RBTree&& other) noexcept;

    bool Insert(float value);
    void Erase(float value);
    void Clear(); //Todo - implement this

    //Todo - const correctness for these?
    bool Contains(float value);
    uint32_t Size();
    uint32_t Height(); //Todo
    bool Empty();
   
    std::vector<float> InOrderTraverse();
    //Todo - const correctness for this?
    Iterator begin();
    Iterator end();
    Iterator Find(float value);
    Iterator LowerBound(float key);
    Iterator UpperBound(float key);

    void Validate();

  private:
    RBTNode* CreateNode(float value, RBTNode* parent);
    void DeleteNode(RBTNode* node);
    bool IsLeftChild(RBTNode* node);
    bool IsRightChild(RBTNode* node);
    uint32_t NumChildren(RBTNode* node);
    RBTNode* GrandParent(RBTNode* node);
    RBTNode* Uncle(RBTNode* node);
    RBTNode* Sibling(RBTNode* node);
    RBTNode* Min(RBTNode* node);
    RBTNode* Max(RBTNode* node);
    RBTNode* Next(RBTNode* node);  
    RBTNode* Previous(RBTNode* node); 
    void Transplant(RBTNode* u, RBTNode* v);
    void SpliceOut(RBTNode* node); 
    void Replace(RBTNode* node, RBTNode* replacement);
    
    void EraseWithoutFixup(RBTNode* node); //Todo - remove this
    void Erase(RBTNode* node);

    void InOrderTraverse(RBTNode* node, std::vector<float>& values_out); 

    void RotateLeft(RBTNode* node);
    void RotateRight(RBTNode* node);
    void InsertFixup(RBTNode* node);

    void DeleteFixup(RBTNode* x);
    void DeleteFixup_0(RBTNode* x);
    void DeleteFixup_1(RBTNode* x);
    void DeleteFixup_2(RBTNode* x);
    void DeleteFixup_3(RBTNode* x);
    void DeleteFixup_4(RBTNode* x);
    
    void Validate(RBTNode* node, uint32_t black_depth, uint32_t depth, 
      std::vector<uint32_t>& black_depths, std::vector<uint32_t>& depths, uint32_t& node_count);

    using BlackHeights = std::unordered_map<RBTNode*, std::vector<int>>;
    using PathStack = std::vector<RBTNode*>;
    void Validate(RBTNode* node, BlackHeights& black_heights, PathStack& path_stack);
   
    std::string PrintColour(RBTNode* node);

  private:
  
    RBTNode* m_root = nullptr;
    RBTNode* m_nil = nullptr;
    uint32_t m_node_count = 0;
  };
}

  namespace rbtree_v2
  {
    template<typename TKey, typename TVal, typename TComp = std::less<TKey>>
    class RBTree
    {
    private:  

      enum class Colour : uint16_t {Red, Black};
      using value_type = std::pair<const TKey,TVal>;

      struct RBNode
      {
        using value_type = typename RBTree::value_type;
        RBNode* left = nullptr;
        RBNode* right = nullptr;
        RBNode* parent = nullptr;
        Colour colour = Colour::Red;
        value_type value;
        RBNode() = default;
        RBNode(const TKey& key, const TVal& val) : value(key,val) {}
        RBNode(const value_type& v) : value(v) {}
        const TKey& Key() {return value.first;}
      };
    
    public:  
      class Iterator
      {
        friend class RBTree; //needs to access RBTree::m_node (private)
      public:
        using value_type    = typename RBTree::value_type;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;  
      public:
        Iterator(RBTree& tree) : m_tree{tree} {}
        Iterator(RBTree& tree, RBNode* node) : m_tree{tree}, m_node{node} {}
        Iterator(const Iterator& other) = default;                //Need this - deleted otherwise
        Iterator& operator = (const Iterator& other) = default;   //Need this - deleted otherwise
        Iterator& operator = (Iterator&& other) noexcept {        //Need this - deleted otherwise
          if(this != &other) {
            m_tree = other.m_tree;
            m_node = other.m_node;
          }
          return *this;
        }
        void Next() {

        }
        void Prev() {

        }
        Iterator& operator++ () { 
          m_node = m_tree.Next(m_node);
          return *this;
        }
        bool operator == (const Iterator& other) const {
          auto tree_addr = std::addressof(m_tree);
          auto tree_addr_other = std::addressof(other.m_tree);
          return (tree_addr == tree_addr_other) && (m_node == other.m_node);
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
        RBTree& m_tree; //Todo - STL iterators don't store a ref to the continer.  Nav logic is embedded in iterator itself
        RBNode* m_node;
      };

    public:
      RBTree();
      explicit RBTree(const std::vector<value_type>& elements);
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

      void Validate();
    
    private:
      void InitSentinal();
      RBNode* CreateNode(const value_type& element, RBNode* parent);
      void DeleteNode(RBNode* node);
      bool IsLeftChild(RBNode* node);
      bool IsRightChild(RBNode* node);
      uint32_t NumChildren(RBNode* node);
      RBNode* GrandParent(RBNode* node);
      RBNode* Uncle(RBNode* node);
      RBNode* Sibling(RBNode* node);
      RBNode* Min(RBNode* node);
      RBNode* Max(RBNode* node);
      RBNode* Next(RBNode* node);  
      RBNode* Previous(RBNode* node);  
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
    
      bool Equal(const TKey& k1, const TKey& k2) {
        return !TComp()(k1,k2) && !TComp()(k2,k1);
      }
      bool Less(const TKey& k1, const TKey& k2) {
        return TComp()(k1,k2);
      }
      TKey Key(value_type& value) {
        return value.first;
      }
      TKey Key(const value_type& value) {
        return value.first;
      }

    private:
      RBNode* m_root;
      uint32_t m_node_count = 0;
      RBNode* m_nil;
    };

    #define RBT_TEMPLATE template<typename TKey, typename TVal, typename TComp>
    #define RBT_TYPE RBTree<TKey,TVal,TComp>


    RBT_TEMPLATE
    RBT_TYPE::RBTree()
    {
      InitSentinal();
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
    RBT_TYPE::RBTree(const std::vector<value_type>& elements)
    {
      InitSentinal();
      for(auto& e : elements) 
        Insert(e);
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
      Iterator itr(*this, node);
      return itr;
    }

    RBT_TEMPLATE
    RBT_TYPE::Iterator RBT_TYPE::end()
    {
      Iterator itr(*this, m_nil);
      return itr;
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
      Iterator itr(*this, node);
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
          return Iterator(*this,node);
        if(!Less(Key(node->value), key)) {
          if( (candidate_node == m_nil) || Less(Key(node->value), Key(candidate_node->value)))
            candidate_node = node;
        }
        if(Less(key, Key(node->value)))
          node = node->left;
        else
          node = node->right;  
      }
      return Iterator(*this,candidate_node); 
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
      return Iterator(*this,candidate_node); 
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
          // SPG_INFO("Node: {}", node->value);
          // for(auto h : black_heights) {
          //   SPG_TRACE("  {}", h);
          // }
          auto max = *(std::max_element(black_heights.begin(), black_heights.end()));
          auto min = *(std::min_element(black_heights.begin(), black_heights.end()));
          if(min != max)
            SPG_ERROR("Node: {}: Min/Max black height: {},{}", Key(node->value), min,max);
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
      //SPG_ASSERT(node != m_root); 
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

     //Todo - should be a member of Iterator - it is only called there
    RBT_TEMPLATE
    RBT_TYPE::RBNode* RBT_TYPE::Next(RBNode* node)
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

    //Todo - should be a member of Iterator - it is only called there
    RBT_TEMPLATE
    RBT_TYPE::RBNode* RBT_TYPE::Previous(RBNode* node)
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
      //values_out.push_back({node->key,node->val});
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

      //i.e. w = Sibling(x);
      // if(IsLeftChild(x)) 
      //   w = x->parent->right;
      // else
      //   w = x->parent->left;
        
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

  } // namespace rbtree_v2

  namespace rbtree_v3
  {
    #define RBT_TEMPLATE template<typename TKey, typename TVal, typename TComp>
    #define RBT_TYPE RBTree<TKey,TVal,TComp>

    template<typename TKey, typename TypeTraits, typename TComp = std::less<TKey>>
    class RBTree
    {
    private:  

      using key_type = TKey;
      using value_type = typename TypeTraits::value_type;
      using reference = value_type&; 
      using const_reference = const value_type&;
      using pointer = value_type*;
      using comparator_type = TComp;

      enum class Colour : uint16_t {Red, Black};
      
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
        friend class RBTree; //needs to access RBTree::m_node (private)
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
      public:
        Iterator(RBNode* node, RBNode* nil) : m_node{node}, m_nil{nil} {}
        Iterator(const Iterator& other) = default;    
        Iterator(Iterator&& other) noexcept = default;                
        Iterator& operator = (const Iterator& other) = default;   
        Iterator& operator = (Iterator&& other) noexcept = default;
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

      void Validate();
    
    private:
      void InitSentinal();
      RBNode* CreateNode(const value_type& element, RBNode* parent);
      void DeleteNode(RBNode* node);
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
      TKey Key(const value_type& value) {return TypeTraits::Key(value);}

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

    template<typename TKey>
    struct SetValueTraits
    {
      using value_type = const TKey;
      static const TKey& Key(const value_type& v) {return v;}
    };

    template<typename TKey, typename TVal, typename TComp = std::less<TKey>>
    using Map = RBTree<TKey, MapValueTraits<TKey, TVal>, TComp>;

    template<typename TKey, typename TComp = std::less<TKey>>
    using Set = RBTree<TKey, SetValueTraits<TKey>, TComp>;
    

  } //namespace rbtree_v3

} //namespace Geom