#include "RBTree.h"

namespace Geom
{
  
  void Test_RB_Tree() 
  {
   
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("Red-Black Tree");
    SPG_WARN("-------------------------------------------------------------------------");
    {
      std::vector<std::pair<const int,int>> map_vals {{2,200},{11,1100},{4,400},{125,12500},{15,1500},{3,300},{9,900},{32,3200},{71,7100},{43,4300},{27,2700},{1,100}};

      RBTree<int, int> my_tree(map_vals);
      my_tree.Validate();
      SPG_TRACE("Node Count: {}:", my_tree.Size())

      //Traversal and iterating
      {
        SPG_WARN("TRAVERSAL AND ITERATING ")
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse");
        my_tree.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        
        SPG_INFO("Ranged for loop traverse");
        for(auto& [key,val] : my_tree) {
          SPG_TRACE("[{},{}]", key, val);
        }

        SPG_INFO("Loop using Iterator")
        for(auto itr = my_tree.begin(); itr != my_tree.end(); ++itr) {
          auto& [key,val] = *itr;
          SPG_TRACE("[{},{}]", key, val);
        }
      }

      {
        SPG_WARN("INSERT ELEMENTS, CHECK VALUE VIA RETURNED ITR ")
        auto it1 = my_tree.Insert({16,161616});
        auto it2 = my_tree.Insert({35,353535});
        SPG_TRACE("Node Count: {}:", my_tree.Size())
        if(it1 != my_tree.end()) 
          SPG_TRACE("[{},{}]", it1->first, it1->second);
        if(it2 != my_tree.end()) 
          SPG_TRACE("[{},{}]", it2->first, it2->second);
        auto it3 = my_tree.Insert({32,323232});  //should be end
        if(it3 == my_tree.end())
          SPG_TRACE("Did not insert {32,323232}")
        for(auto& [key,val] : my_tree) SPG_TRACE("[{},{}]", key, val);
        SPG_WARN("ERASING ELEMENTS ADDED: ") 
        my_tree.Erase(it1);
        my_tree.Erase(it2);
        for(auto& [key,val] : my_tree) SPG_TRACE("[{},{}]", key, val);
        SPG_TRACE("Node Count: {}:", my_tree.Size())
      }

      //Find(), Contains()
      {
        SPG_WARN("FIND, CONTAINS ")
        for(auto& [key, value]: map_vals) {
          SPG_INFO("Contains {}? {}", key, my_tree.Contains(key));
        }
        SPG_INFO("Contains {}? {}", -15, my_tree.Contains(-15));
        SPG_INFO("Contains {}? {}", 0, my_tree.Contains(0));
        SPG_INFO("Contains {}? {}", 160, my_tree.Contains(160));
        SPG_INFO("Contains {}? {}", 33, my_tree.Contains(33));
      }

      {
        SPG_WARN("LOWERBOUND UPPERBOUND ")
        for(auto& [key, value]: map_vals) {
          auto itr = my_tree.LowerBound(key);
          if(itr == my_tree.end()) {
            SPG_INFO("LowerBound {}? end", key);
          }
          else {
            SPG_INFO("LowerBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_tree.LowerBound(k);
          if(itr == my_tree.end()) {
            SPG_INFO("LowerBound {}? end", k);
          }
          else {
            SPG_INFO("LowerBound {}? {}", k, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          auto itr = my_tree.UpperBound(key);
          if(itr == my_tree.end()) {
            SPG_INFO("UpperBound {}? end", key);
          }
          else {
            SPG_INFO("UpperBound {}? {}", key, (*itr).first);
          }
        }
        for(auto& [key, value]: map_vals) {
          int k = key-10;
          auto itr = my_tree.UpperBound(k);
          if(itr == my_tree.end()) {
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
        my_tree.Erase(1);
        my_tree.Erase(71);
        my_tree.Erase(27);
        std::vector<std::pair<const int,int>> elements_out;
        SPG_INFO("In Order Traverse After erase");
        my_tree.InOrderTraverse(elements_out);
        for(auto& [key,val]: elements_out) {
          SPG_TRACE("[{},{}]", key, val);
        }
        my_tree.Validate();
        SPG_TRACE("Node Count: {}:", my_tree.Size())
      }

      //Stress test with large data, insertion, erasure, clear
      {
        SPG_WARN("STRESS TEST WITH LARGE AMOUNT OF DATA ")
        my_tree.Clear();
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
          if(my_tree.Insert(element) != my_tree.end()) {
            rb_values.push_back(val);
            rb_values_to_delete.push_back(false);    
          }
        }
        my_tree.Validate();

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
          my_tree.Erase(rb_values[i]);
        }
        my_tree.Validate();
        SPG_INFO("Clearing Tree");
        my_tree.Clear();
        my_tree.Validate();
        SPG_INFO("Tree Size {}: ", my_tree.Size());

        SPG_INFO("Add a few more elements");
        for(auto& element : map_vals)
          my_tree.Insert(element);
        SPG_INFO("Ranged for loop traverse");
        for(auto& [key,val] : my_tree) {
          SPG_TRACE("[{},{}]", key, val);
        }
        SPG_INFO("Tree Size {}: ", my_tree.Size());
        my_tree.Validate();
      }
    
      //Copy move constructor, assignment operator
      {
        auto t2 = my_tree; //copy constructor
        Geom::RBTree<int,int> t3;
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
        Geom::RBTree<int,int> t5;
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
        Geom::RBTree<float,void> rb_tree;
        for(auto v : vals)
          rb_tree.Insert(v);
        rb_tree.Validate();  
        rb_tree.InOrderTraverse(ordered_vals_out);
        SPG_WARN("Elements in RBTree {}", rb_tree.Size())
        for(auto& v : ordered_vals_out) {
          SPG_TRACE(v);
        }
        SPG_WARN("cLEARING")
        rb_tree.Clear();
        rb_tree.Validate();
      }
      {
         SPG_WARN("ADDING THEN DELETING A SINGLE ELEMENT ")
         RBTree<float,void> tree;
         tree.Insert(1.0f);
         tree.Insert(2.0f);
         tree.Insert(3.0f);
         tree.Validate();
         tree.Erase(2.0f);
         tree.Erase(1.0f);
         for(auto& v : tree) SPG_TRACE(v);
         tree.Erase(3.0f);
         tree.Validate(); 
      }
    }
 }

  namespace RBTree_V2 
  {

    struct IntTreeNode : RBNodeBase<IntTreeNode,int>
    {
      using Base = RBNodeBase<IntTreeNode,int>;
      using Base::Base;
    };

    void Test_RBTree() {

      SPG_WARN("-------------------------------------------------------------------------");
      SPG_WARN("RBTree_V2");
      SPG_WARN("-------------------------------------------------------------------------");
      {
        std::vector<int> tree_vals {2,11,4,125,15,3,9,32,71,43,27,1};

        RBTree<int> my_tree(tree_vals);
        //RBTree<int> my_tree; //ok
        //RBTree<int,std::less<int>,IntTreeNode> my_tree; //okay
        my_tree.Validate();
        SPG_TRACE("Tree Size: {}:", my_tree.Size())

        //Traversal and iterating
        {
          SPG_WARN("TRAVERSAL AND ITERATING ")
          std::vector<int> elements_out;
          SPG_INFO("In Order Traverse");
          my_tree.InOrderTraverse(elements_out);
          for(auto& val: elements_out) {
            SPG_TRACE("[{}]",val);
          }
          
          SPG_INFO("Ranged for loop traverse");
          for(auto& val : my_tree) {
            SPG_TRACE("[{}]", val);
          }

          SPG_INFO("Loop using Iterator")
          for(auto itr = my_tree.begin(); itr != my_tree.end(); ++itr) {
            auto& val = *itr;
            SPG_TRACE("[{}]", val);
          }
        }
      
        {
          SPG_WARN("INSERT ELEMENTS, CHECK VALUE VIA RETURNED ITR ")
          auto it1 = my_tree.Insert(16);
          auto it2 = my_tree.Insert(35);
          SPG_TRACE("Tree Size: {}:", my_tree.Size())
          if(it1 != my_tree.end()) 
            SPG_TRACE("[{}]", *it1);
          if(it2 != my_tree.end()) 
            SPG_TRACE("[{}]", *it2);
          auto it3 = my_tree.Insert(32);  //should be end
          if(it3 == my_tree.end())
            SPG_TRACE("Did not insert 32")
          for(auto& val : my_tree) SPG_TRACE("[{}]",val);
          SPG_WARN("ERASING ELEMENTS ADDED: ");
          my_tree.Erase(it1);
          my_tree.Erase(it2);
          for(auto& val : my_tree) SPG_TRACE("[{}]", val);
          SPG_TRACE("Tree Size: {}:", my_tree.Size())
        }

        //Find(), Contains()
        {
          SPG_WARN("FIND, CONTAINS ")
          for(auto& val: tree_vals) {
            SPG_TRACE("Contains {}? {}", val, my_tree.Contains(val));
          }
          SPG_TRACE("Contains {}? {}", -15, my_tree.Contains(-15));
          SPG_TRACE("Contains {}? {}", 0, my_tree.Contains(0));
          SPG_TRACE("Contains {}? {}", 160, my_tree.Contains(160));
          SPG_TRACE("Contains {}? {}", 33, my_tree.Contains(33));
        }

        // lowerbound(), Upperbound()
        {
          SPG_WARN("LOWERBOUND UPPERBOUND ")
          for(auto& val: tree_vals) {
            auto itr = my_tree.LowerBound(val);
            if(itr == my_tree.end()) {
              SPG_TRACE("LowerBound {}? end", val);
            }
            else {
              SPG_TRACE("LowerBound {}? {}", val, *itr);
            }
          }
          for(auto& val: tree_vals) {
            int v = val-10;
            auto itr = my_tree.LowerBound(v);
            if(itr == my_tree.end()) {
              SPG_TRACE("LowerBound {}? end", v);
            }
            else {
              SPG_TRACE("LowerBound {}? {}", v, *itr);
            }
          }
          for(auto& val: tree_vals) {
            auto itr = my_tree.UpperBound(val);
            if(itr == my_tree.end()) {
              SPG_TRACE("UpperBound {}? end", val);
            }
            else {
              SPG_TRACE("UpperBound {}? {}", val, *itr);
            }
          }
          for(auto& val: tree_vals) {
            int v = val-10;
            auto itr = my_tree.UpperBound(v);
            if(itr == my_tree.end()) {
              SPG_TRACE("UpperBound {}? end", v);
            }
            else {
              SPG_TRACE("UpperBound {}? {}", v, *itr);
            }
          }
          
        }

        //Deletion
        {
          SPG_WARN("DELETION ")
          my_tree.Erase(1);
          my_tree.Erase(71);
          my_tree.Erase(27);
          std::vector<int> elements_out;
          SPG_INFO("In Order Traverse After erase");
          my_tree.InOrderTraverse(elements_out);
          for(auto& val: elements_out) {
            SPG_TRACE("[{}]",val);
          }
          my_tree.Validate();
          SPG_TRACE("Tree Size: {}:", my_tree.Size())
        }

        //Stress test with large data, insertion, erasure, clear
        {
          SPG_WARN("STRESS TEST WITH LARGE AMOUNT OF DATA ")
          my_tree.Clear();
          //Add a bunch of random values
          std::vector<int> rb_values;
          std::vector<bool> rb_values_to_delete;
        
          const uint32_t RB_NUM_VALS = 10000;
          const int RB_MIN_VAL = -1000000;
          const int RB_MAX_VAL = 1000000;
      
          std::random_device rd;                         
          std::mt19937 mt(rd()); 
          std::uniform_int_distribution<int> dist(RB_MIN_VAL, RB_MAX_VAL); 
        
          for(int i=0; i< RB_NUM_VALS; i++) {
            int val = dist(mt);
            if(my_tree.Insert(val) != my_tree.end()) {
              rb_values.push_back(val);
              rb_values_to_delete.push_back(false);    
            }
          }
          my_tree.Validate();
          SPG_TRACE("Tree Size: {}:", my_tree.Size())

          //pick values to delete at random
          std::uniform_int_distribution<int> idist(0, rb_values.size()-1);  
          const uint32_t RB_NUM_VALS_TO_DEL = my_tree.Size()/2;
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
            my_tree.Erase(rb_values[i]);
          }
          my_tree.Validate();
          SPG_TRACE("Tree Size {}: ", my_tree.Size());
          SPG_INFO("Clearing Tree");
          my_tree.Clear();
          my_tree.Validate();
          SPG_TRACE("Tree Size {}: ", my_tree.Size());

          SPG_INFO("Add a few more elements");
          for(auto& val : tree_vals)
            my_tree.Insert(val);
          SPG_INFO("Ranged for loop traverse");
          for(auto& val : my_tree) {
            SPG_TRACE("[{}]", val);
          }
          my_tree.Validate();
          SPG_TRACE("Tree Size: {}:", my_tree.Size())
        }
      
        //Copy move constructor, assignment operator
        {
          SPG_WARN("COPY AND MOVE ")
          auto t2 = my_tree; //copy constructor
          RBTree<int> t3;
          t3 = t2; //copy assignment;
          SPG_INFO("t2:");
          for(auto& val : t2) {
            SPG_TRACE("[{}]", val);
          }
          SPG_INFO("t3:");
          for(auto& val : t3) {
            SPG_TRACE("[{}]", val);
          }
          auto t4 = std::move(t2); //move ctr
          RBTree<int> t5;
          t5 = std::move(t3); //move assignment

          SPG_INFO("After move:");
          SPG_INFO("t2:");
          for(auto& val : t2) {
            SPG_TRACE("[{}]", val);
          }
          SPG_INFO("t3:");
          for(auto& val : t3) {
            SPG_TRACE("[{}]", val);
          }
          SPG_INFO("t4:");
          for(auto& val : t4) {
            SPG_TRACE("[{}]", val);
          }
          SPG_INFO("t5:");
          for(auto& val : t5) {
            SPG_TRACE("[{}]", val);
          }
        }
#if 0
#endif

#if 1
        {
          SPG_WARN("IN-ORDER TRAVERSAL OF BALANCED TREE ")
          std::vector<float> vals{26,32,43,11,15,100,17,7,87,42,150,111, 27, 54,1,33,200,88,99,0};
          std::vector<float> ordered_vals_out;
          RBTree<float> rb_tree;
          for(auto v : vals)
            rb_tree.Insert(v);
          rb_tree.Validate();  
          rb_tree.InOrderTraverse(ordered_vals_out);
          SPG_WARN("Elements in RBTree {}", rb_tree.Size())
          for(auto& v : ordered_vals_out) {
            SPG_TRACE(v);
          }
          SPG_WARN("CLEARING")
          rb_tree.Clear();
          rb_tree.Validate();
        }
        {
          SPG_WARN("ADDING THEN DELETING A SINGLE ELEMENT ")
          RBTree<float> tree;
          tree.Insert(1.0f);
          tree.Insert(2.0f);
          tree.Insert(3.0f);
          tree.Validate();
          tree.Erase(2.0f);
          tree.Erase(1.0f);
          for(auto& v : tree) SPG_TRACE(v);
          tree.Erase(3.0f);
          tree.Validate(); 
        }
#endif
      }

    }
  }

}