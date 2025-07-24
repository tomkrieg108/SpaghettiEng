#pragma once
#include <Common/Common.h>

namespace Geom
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
    void SpliceOut(RBTNode* node); 
    void Replace(RBTNode* node, RBTNode* replacement);
    void Transplant(RBTNode* u, RBTNode* v);

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