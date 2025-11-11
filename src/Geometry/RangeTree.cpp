#include "RangeTree.h"
#include "RBTree.h"

namespace Geom
{
  std::vector<float> RangeTree1D::RangeSearch(const Range& range)
  {
    std::vector<float> vals_out;

    auto itr_split = m_tree.FindSplitPos(range.x_min, range.x_max);
    if( itr_split == m_tree.end())
      return vals_out;
    
    Search(itr_split, range, vals_out);
    return vals_out;
  }

   void RangeTree1D::Search(Iterator itr, const Range& range, std::vector<float>& out)
   {
    if(itr == m_tree.end())
      return;
    if((*itr > range.x_min) && (m_tree.LeftChild(itr) != m_tree.end())) 
      Search(m_tree.LeftChild(itr),range,out);
    if((*itr >= range.x_min) && (*itr <= range.x_max)) 
      out.push_back(*itr);
    if((*itr < range.x_max) && (m_tree.RightChild(itr) != m_tree.end())) 
      Search(m_tree.RightChild(itr),range,out);
   }


  void RangeTree1D::Test()
  {
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("RangeTree1D - Test");
    SPG_WARN("-------------------------------------------------------------------------");
    //std::vector<float> vals {2,11,4,125,15,3,9,32,71,43,27,1};
    std::vector<float> vals;

    //Add a bunch of random values
    const uint32_t KD_NUM_VALS = 100;
    const float KD_MIN_VAL = 0;
    const float KD_MAX_VAL = 200;
    
    std::random_device rd;                         
    std::mt19937 mt(rd()); 
    std::uniform_real_distribution<float> fdist(KD_MIN_VAL, KD_MAX_VAL); 
    
    for(int i=0; i< KD_NUM_VALS; i++) {
      vals.push_back(fdist(mt));
    }

    RangeTree1D range_tree(vals);
    SPG_INFO("All values");
    for(auto& e : range_tree) 
      SPG_TRACE(e);

    Range range{45,55};
    //Range range{-10,1};
    //Range range{31,125};
    //Range range{150,200};
    
    std::vector<float> in_range;
    SPG_INFO("Values in range: [{},{}] ", range.x_min, range.x_max);
    in_range = range_tree.RangeSearch(range);
    std::sort(in_range.begin(), in_range.end());
    for(auto& e : in_range)
      SPG_TRACE(e);  
  }

//-------------------------------------------------------------------------------
// RangeTree2D
//-------------------------------------------------------------------------------

  RangeTree2D::RangeTree2D( std::vector<Geom::Point2d>& points)
  {
     if(points.empty())
      return;
    std::sort(points.begin(), points.end(), [](Point2d a, Point2d b) {return a.x < b.x;});  
    m_root = BuildTree(points);  
  }

  RangeTree2D::RangeTree2D(std::vector<Geom::Point2d>&& points) noexcept
  {
     if(points.empty())
      return;
      std::sort(points.begin(), points.end(), [](Point2d a, Point2d b) {return a.x < b.x;});  
      m_root = BuildTree(std::move(points));  
  }

  RangeTree2D::Node* RangeTree2D::BuildTree(std::vector<Point2d> points)
  {
    uint32_t num_points = points.size();
    SPG_ASSERT(num_points > 0);
    if(num_points == 1) {
      Node* node = new Node();
      node->is_leaf = true;
      node->point = points[0];
      node->secondary_tree = SecondaryTree(points);
      return node;
    }

    //Todo - check that the median element is only in the left half (not both) for both odd and even number of points
    //Todo - figure out why it crashes when using the commented out version
    //Todo - ensure comparators and median choice agree, ensure points that have equal x (and maybe y), median splitting assigns them consistently to L or R to avoid missing points
    //Assume points are sorted by x-coord
    uint32_t median_pos = (num_points%2 == 0) ? num_points/2 : num_points/2 + 1;
    //uint32_t median_pos = (num_points%2 == 0) ? num_points/2 - 1 : num_points/2; //this causes a crash !?
    float split_value = points[median_pos-1].x;
    //float split_value = points[median_pos].x;

    std::vector<Point2d> first_half(points.begin(), points.begin() + median_pos);
    std::vector<Point2d> second_half(points.begin() + median_pos, points.end());  

    Node* node = new Node();
    node->is_leaf = false;
    node->x_val = split_value;
    node->secondary_tree = SecondaryTree(points);
    node->left = BuildTree(std::move(first_half));
    node->right = BuildTree(std::move(second_half));
    return node;
  }

  RangeTree2D::Node* RangeTree2D::FindSplitNode(float x_low, float x_high)
  {
    //Todo either return nullptr if low < high or swap the value
    SPG_ASSERT(x_low < x_high);
    SPG_ASSERT(m_root != nullptr); //todo return nullptr instead
    Node* node = m_root;
    while( (node != nullptr) && (!node->is_leaf) && ((x_high <= node->x_val) || (x_low > node->x_val)) ) {
      if(x_high <= node->x_val)
        node = node->left;
      else
        node = node->right;  
    }
    return node;
  }

  std::vector<Point2d> RangeTree2D::RangeQueryY(SecondaryTree& tree, const Range& range)
  {
    //Only the y-coord in Point2d is used (secondary tree ordered on y coord), but key needs to be Point2d
    auto itr_split = tree.FindSplitPos(Point2d{range.x_min,range.y_min},    Point2d{range.x_max,range.y_max} );
    std::vector<Point2d> points_out;
    if(itr_split == tree.end())
      return points_out;
    SeachSubTreeSecondary(tree, itr_split, range, points_out);
    return points_out;
  }

  void RangeTree2D::SeachSubTreeSecondary(SecondaryTree& tree,Iterator itr , const Range& range,std::vector<Point2d>& points_out)
  {
    if(itr == tree.end())
      return;
    const auto& point = *itr;  
    if((point.y > range.y_min) && (tree.LeftChild(itr) != tree.end()))
      SeachSubTreeSecondary(tree, tree.LeftChild(itr), range, points_out);
    if(PointInRange(point, range))
      points_out.push_back(point);
    if((point.y < range.y_max) && (tree.RightChild(itr) != tree.end()) )
      SeachSubTreeSecondary(tree, tree.RightChild(itr), range, points_out);
  }
  
  std::vector<Point2d> RangeTree2D::RangeQuery(const Range& range)
  {
    std::vector<Point2d> points;
    Node* split_node = FindSplitNode(range.x_min, range.x_max);
    if(split_node == nullptr)
      return points;
    
    if(split_node->is_leaf) {
      if(PointInRange(split_node->point, range))
        points.push_back(split_node->point);
      return points;  
    }

    Node* node = split_node->left;
    while (!node->is_leaf) {
      if(range.x_min <= node->x_val) {
        SPG_ASSERT(node->right != nullptr);
        auto points_in_range = RangeQueryY(node->right->secondary_tree,range);
        points.insert(points.end(), points_in_range.begin(), points_in_range.end());
        node=node->left;
      }
      else
        node = node->right;
    }
    if(PointInRange(node->point, range))
      points.push_back(node->point);

    node = split_node->right;
    while (!node->is_leaf) {
      if(range.x_max > node->x_val) {
        SPG_ASSERT(node->left != nullptr);
        auto points_in_range = RangeQueryY(node->left->secondary_tree,range);
        points.insert(points.end(), points_in_range.begin(), points_in_range.end());
        node=node->right;
      }
      else
        node = node->left;
    }
    if(PointInRange(node->point, range))
      points.push_back(node->point);
    
    return points;
  }

  bool RangeTree2D::PointInRange(Point2d p, const Range& range)
  {
    //Todo - make sure the inequalities match intended semantics (inclusive ve exclusive bounds).  x<x_max (exclusive upper bound) x >= x_min (inclusive upper bound)
    if(!(p.x < range.x_max))
      return false;
    if(p.x < range.x_min)
      return false;
    if(!(p.y < range.y_max))
      return false;
    if(p.y < range.y_min)
      return false;

    return true;   
  }

  //================================================================================
  // Following is used for test / validation only
  //================================================================================

  void RangeTree2D::ReportSubTreeMain(Node* node, std::vector<Point2d>& out_points)
   {
    if(node == nullptr)
      return;
    if(node->is_leaf) 
      out_points.push_back(node->point);
    else {
      ReportSubTreeMain(node->left,out_points);
      ReportSubTreeMain(node->right,out_points);
    }    
   }

  //Report points that fall in the x range - ignore y range
  std::vector<Point2d> RangeTree2D::RangeQueryX(const Range& range)
  {
    std::vector<Point2d> points;
    Node* split_node = FindSplitNode(range.x_min, range.x_max);
    if(split_node == nullptr)
      return points;
    
    if(split_node->is_leaf) {
      if(split_node->point.x >= range.x_min && split_node->point.x <= range.x_max)
        points.push_back(split_node->point);
      return points;  
    }

    Node* node = split_node->left;
    while (!node->is_leaf) {
      if(range.x_min <= node->x_val) {
        ReportSubTreeMain(node->right, points);
        node=node->left;
      }
      else
        node = node->right;
    }
    if(node->point.x >= range.x_min && node->point.x <= range.x_max)
      points.push_back(node->point);

    node = split_node->right;
    while (!node->is_leaf) {
      if(range.x_max > node->x_val) {
        ReportSubTreeMain(node->left, points);
        node=node->right;
      }
      else
        node = node->left;
    }
    if(node->point.x >= range.x_min && node->point.x <= range.x_max)
      points.push_back(node->point);
    
    return points;
  } 

  std::vector<Point2d> RangeTree2D::BruteForceRangeQuery(const Range& range) 
  {
    std::vector<Point2d> all_points;
    ReportSubTreeMain(m_root, all_points);
    std::vector<Point2d> points_in_range;
    for(auto& p : all_points) {
      if(PointInRange(p,range))
        points_in_range.push_back(p);
    }
    return points_in_range;
  }

  void RangeTree2D::ValidateTree(Node* node)
  {
    if(node == nullptr)
      return;

    std::vector<Point2d> points_primary;
    std::vector<Point2d> points_secondary;

    ReportSubTreeMain(node, points_primary);  

    SPG_INFO("Subtree at split-x={},  Subtree vals={}, 2nd Tree Vals={}", 
      node->x_val, points_primary.size(), node->secondary_tree.Size());
    for(auto& e : node->secondary_tree) 
      points_secondary.push_back(e);

    SPG_ASSERT(points_primary.size() == points_secondary.size());
    std::sort(points_primary.begin(), points_primary.end(), CompX()); //already sorted by x-coord anyway!
    std::sort(points_secondary.begin(), points_secondary.end(), CompX()); //initialliy sorted by y-coord 
    for(auto i=0; i<points_primary.size(); ++i ){
      SPG_TRACE("{}: P:{} S:{}",i+1, points_primary[i], points_secondary[i]);
      SPG_ASSERT(Geom::Equal(points_primary[i], points_secondary[i]));
    }

    if(!node->is_leaf) {
      ValidateTree(node->left);
      ValidateTree(node->right);
    }  
  }

  void RangeTree2D::Test()
  {
    SPG_WARN("-------------------------------------------------------------------------");
    SPG_WARN("RangeTree2D - Test");
    SPG_WARN("-------------------------------------------------------------------------");
    std::vector<Geom::Point2d> points;

    //Add a bunch of random values
    const uint32_t KD_NUM_VALS = 1000;
    const float KD_MIN_VAL = 0;
    const float KD_MAX_VAL = 1000;
    
    std::random_device rd;                         
    std::mt19937 mt(rd()); 
    std::uniform_real_distribution<float> fdist(KD_MIN_VAL, KD_MAX_VAL); 
    
    for(int i=0; i< KD_NUM_VALS; i++) {
      Geom::Point2d p(fdist(mt),fdist(mt));
      points.push_back(p);
    }

    RangeTree2D tree(points);
    //tree.ValidateTree(tree.m_root);

    RangeTree2D::Range range{400,500,400,500};

    std::vector<Geom::Point2d> points_in_range;
    points_in_range = tree.RangeQuery(range); 
    SPG_WARN("Points in range X:[{},{}] Y:[{},{}]", range.x_min, range.x_max, range.y_min, range.y_max);
    for(auto& p: points_in_range)
      SPG_TRACE(p);

    std::vector<Point2d> points_bf;
    points_bf = tree.BruteForceRangeQuery(range);
    SPG_WARN("Points in range (BF) X:[{},{}] Y:[{},{}]", range.x_min, range.x_max, range.y_min, range.y_max);
    for(auto& p: points_bf)
      SPG_TRACE(p);

    SPG_WARN("COMPARING BRUTE FORCE Query AND RangeQuery...");
    SPG_ASSERT(points_bf.size() == points_in_range.size());
    std::sort(points_in_range.begin(), points_in_range.end(), Comp());
    std::sort(points_bf.begin(), points_bf.end(), Comp());
    for(auto i=0; i< points_bf.size(); ++i) 
      SPG_ASSERT(Geom::Equal(points_bf[i], points_in_range[i]));
    SPG_WARN("Comparison check out!");
  }
}