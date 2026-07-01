
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "Geometry/Geometry.h"
#include "MathLib/Geom/Geom.h"

namespace GeomTest 
{
  //#define RUN_BENCHMARKS
 
  namespace CM = Catch::Matchers;

  TEST_CASE( "Float equality test", "Equal") {
    float a = 2.57630f; float b = 2.57631f; float c = 2.57632f;
    REQUIRE(SpgMth::Equal(a,b) == true);
    REQUIRE(SpgMth::Equal(b,c) == true);
    REQUIRE(SpgMth::Equal(a,c) == false); //differ by 0.00002 > SpgMth::Epsilon(100) = // 0.000019
    REQUIRE(SpgMth::Equal(a,c,1000.0f) == true);

  #if defined(RUN_BENCHMARKS) 
    BENCHMARK("Float Equality benchmark") {
        return SpgMth::Equal(a,b,100.0f);
    };
  #endif  
  }

  TEST_CASE( "Point2d equality test", "Equal") {
    SpgMth::Point2d p1{1.634, -5.345}, p2{1.634254f, -5.345168f}, p3{1.6342543f, -5.3451648f};
    REQUIRE(SpgMth::Equal(p1,p2) == false);
    REQUIRE(SpgMth::Equal(p2,p3) == true);
    REQUIRE(SpgMth::Equal(p1,p3,10000.0f) == true); //equal to 3 decimal places true
    REQUIRE(SpgMth::Equal(p2,p3, 10.0f) == false); //equal to 6 decimal places false

  #if defined(RUN_BENCHMARKS)   
     BENCHMARK("Point2d Equality benchmark") {
        return SpgMth::Equal(p1,p2,100.0f);
    };
  #endif  

  }

  TEST_CASE( "SignedArea", "[ComputeSignedArea]" ) {
    SpgMth::Point2d A{-2.96,-1.48}, B{5.044,1.43}, C{0.86,1.36};
    auto expected = 5.80758; auto percent = 0.001; const auto epsilon = SpgMth::Epsilon(100); // 0.000019 same to 4 decimal places =? equal
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,B,C), CM::WithinRel(expected, percent) || CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(B,C,A), CM::WithinRel(expected, percent) || CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(C,A,B), CM::WithinRel(expected, percent) || CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,C,B), CM::WithinRel(-expected, percent) || CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(B,A,C), CM::WithinRel(-expected, percent) || CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(C,B,A), CM::WithinRel(-expected, percent) || CM::WithinAbs(-expected, epsilon) );  //CW

    //near zero area
    C = {-1.556, -0.946}; expected = 0.09425; percent = 0.001;
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,B,C), CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(B,C,A), CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(C,A,B), CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,C,B), CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(B,A,C), CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( SpgMth::ComputeSignedArea(C,B,A), CM::WithinAbs(-expected, epsilon) );  //CW

   //zero area (colinear)
    A={-6,2}, B={-1.622,1.276}, C={-4.60752,1.76972};  expected = 0; percent = 0.001;
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,B,C),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( SpgMth::ComputeSignedArea(B,C,A),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( SpgMth::ComputeSignedArea(C,A,B),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,C,B),  CM::WithinAbs(-expected, epsilon) );  
    REQUIRE_THAT( SpgMth::ComputeSignedArea(B,A,C),  CM::WithinAbs(-expected, epsilon) );  
    REQUIRE_THAT( SpgMth::ComputeSignedArea(C,B,A),  CM::WithinAbs(-expected, epsilon) );  

    //coincident point(s)
    expected = 0; percent = 0.001;
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,A,B),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,B,B),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( SpgMth::ComputeSignedArea(A,A,A),  CM::WithinAbs(expected, epsilon) );  
  }

  TEST_CASE( "Orientation2d", "[Orientation2d]" ) {
    SpgMth::Point2d A{-2.96,-1.48}, B{5.044,1.43}, C{-3.02,0.924};
    REQUIRE(SpgMth::Orientation2d(A,B,C) == SpgMth::RelativePos::Left);
    REQUIRE(SpgMth::Orientation2d(B,C,A) == SpgMth::RelativePos::Left);
    REQUIRE(SpgMth::Orientation2d(C,A,B) == SpgMth::RelativePos::Left);
    REQUIRE(SpgMth::Orientation2d(A,C,B) == SpgMth::RelativePos::Right);
    REQUIRE(SpgMth::Orientation2d(C,B,A) == SpgMth::RelativePos::Right);
    REQUIRE(SpgMth::Orientation2d(B,A,C) == SpgMth::RelativePos::Right);

   #if defined(RUN_BENCHMARKS)    
    BENCHMARK("Orientation2d benchmark - left") {
        return SpgMth::Orientation2d(A,B,C);
    };
   #endif 

    //Near Colinear
    C={-0.918,-0.704};
    REQUIRE(SpgMth::Orientation2d(A,B,C) == SpgMth::RelativePos::Left);
    REQUIRE(SpgMth::Orientation2d(B,C,A) == SpgMth::RelativePos::Left);
    REQUIRE(SpgMth::Orientation2d(C,A,B) == SpgMth::RelativePos::Left);
    REQUIRE(SpgMth::Orientation2d(A,C,B) == SpgMth::RelativePos::Right);
    REQUIRE(SpgMth::Orientation2d(C,B,A) == SpgMth::RelativePos::Right);
    REQUIRE(SpgMth::Orientation2d(B,A,C) == SpgMth::RelativePos::Right);

    //coincident endpoints
    REQUIRE(SpgMth::Orientation2d(A,B,A) == SpgMth::RelativePos::Origin);
    REQUIRE(SpgMth::Orientation2d(A,B,B) == SpgMth::RelativePos::Destination);

    //Collinear case - negative slope for line seg
    A={-6,2}, B={-2.018,1.232}, C={-3.93443,1.60162}; //C is between A and B (colinear and interior)
    REQUIRE(SpgMth::Orientation2d(A,B,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(B,A,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(A,C,B) == SpgMth::RelativePos::Beyond);
    REQUIRE(SpgMth::Orientation2d(C,B,A) == SpgMth::RelativePos::Behind);

    //Collinear case - positive slope for line seg
    A={-6,1}, B={-2.19875,2.33779}, C={-3.89479,1.7409}; //C is between A and B (colinear and interior)
    REQUIRE(SpgMth::Orientation2d(A,B,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(B,A,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(A,C,B) == SpgMth::RelativePos::Beyond);
    REQUIRE(SpgMth::Orientation2d(C,B,A) == SpgMth::RelativePos::Behind);

    //Horizontal line
    A={-6,1}, B={-2,1}, C={-4.66632,1}; //C is between A and B (collinear and interior)
    REQUIRE(SpgMth::Orientation2d(A,B,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(B,A,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(A,C,B) == SpgMth::RelativePos::Beyond);
    REQUIRE(SpgMth::Orientation2d(C,B,A) == SpgMth::RelativePos::Behind);

    //Vertical line
     A={-2,3}, B={-2,1}, C={-2,2.03742}; //C is between A and B (collinear and interior)
    REQUIRE(SpgMth::Orientation2d(A,B,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(B,A,C) == SpgMth::RelativePos::Between);
    REQUIRE(SpgMth::Orientation2d(A,C,B) == SpgMth::RelativePos::Beyond);
    REQUIRE(SpgMth::Orientation2d(C,B,A) == SpgMth::RelativePos::Behind);

  #if defined(RUN_BENCHMARKS)     
    BENCHMARK("Orientation2d benchmark - between") { 
      return SpgMth::Orientation2d(A,B,C);
    };
  #endif  

  }

  TEST_CASE( "Colinearity 2d", "[Collinear]" ) {
  //Collinear case - negative slope for line seg
  SpgMth::Point2d A={-6,2}, B={-2.018,1.232}, C={-3.93443,1.60162}; //C is between A and B (colinear and interior)
  REQUIRE(SpgMth::Collinear(A,B,C) == true);
  REQUIRE(SpgMth::Collinear(B,A,C) == true);
  REQUIRE(SpgMth::Collinear(A,C,B) == true);
  REQUIRE(SpgMth::Collinear(C,B,A) == true);
  REQUIRE(SpgMth::Collinear(A,A,B) == true);
  REQUIRE(SpgMth::Collinear(A,B,A) == true);
  REQUIRE(SpgMth::Collinear(B,A,A) == true);
  REQUIRE(SpgMth::Collinear(A,A,A) == true);

  //Collinear case - positive slope for line seg
  A={-6,1}, B={-2.19875,2.33779}, C={-3.89479,1.7409}; //C is between A and B (colinear and interior)
  REQUIRE(SpgMth::Collinear(A,B,C) == true); 
  REQUIRE(SpgMth::Collinear(B,A,C) == true);
  REQUIRE(SpgMth::Collinear(A,C,B) == true);
  REQUIRE(SpgMth::Collinear(C,B,A) == true);

  //Horizontal line
  A={-6,1}, B={-2,1}, C={-4.66632,1}; //C is between A and B (collinear and interior)
  REQUIRE(SpgMth::Collinear(A,B,C) == true);
  REQUIRE(SpgMth::Collinear(B,A,C) == true);
  REQUIRE(SpgMth::Collinear(A,C,B) == true);
  REQUIRE(SpgMth::Collinear(C,B,A) == true);

  //Vertical line
    A={-2,3}, B={-2,1}, C={-2,2.03742}; //C is between A and B (collinear and interior)
  REQUIRE(SpgMth::Collinear(A,B,C) == true);
  REQUIRE(SpgMth::Collinear(B,A,C) == true);
  REQUIRE(SpgMth::Collinear(A,C,B) == true);
  REQUIRE(SpgMth::Collinear(C,B,A) == true);

  //Near collinear
  A={-2.96,-1.48}, B={5.044,1.43}, C={-0.918,-0.704};
  REQUIRE(SpgMth::Collinear(A,B,C) == false);
  REQUIRE(SpgMth::Collinear(B,C,A) == false);
  REQUIRE(SpgMth::Collinear(C,A,B) == false);
  REQUIRE(SpgMth::Collinear(A,C,B) == false);
  REQUIRE(SpgMth::Collinear(C,B,A) == false);
  REQUIRE(SpgMth::Collinear(B,A,C) == false);

#if defined(RUN_BENCHMARKS)     
  BENCHMARK("Collinearity 2d benchmark") { 
    return SpgMth::Collinear(A,B,C);
  };
#endif  

  }

  //Todo - test ComputeIntersection() 
  TEST_CASE( "Intersection Exists", "bool IntersectionExists(const LineSeg2D&, const LineSeg2D&)") {

  SpgMth::LineSeg2D L1{{20,30},{60,20}}, L2{{50,30},{30,20}};
  REQUIRE(SpgMth::IntersectionExists(L1,L2) == true); //Segments cross
  REQUIRE(SpgMth::IntersectionExists(L2,L1) == true); //Segments cross
  //Todo: this test fails
  L1={{20,30},{70,31}}; L2={{50,30},{30,20}};
  REQUIRE(SpgMth::IntersectionExists(L1,L2) == true); //Coinciding endpoints. 
  REQUIRE(SpgMth::IntersectionExists(L2,L1) == true); //Coinciding endpoints
  L1={{20,30},{70,31}}; L2={{10,27},{80,33}};
  REQUIRE(SpgMth::IntersectionExists(L1,L2) == true); //Segments cross
  REQUIRE(SpgMth::IntersectionExists(L2,L1) == true); //Segments cross
  L1={{20,30},{60,20}}; L2={{30,10},{46.5143085967805f,23.3714228508049f}}; 
  REQUIRE(SpgMth::IntersectionExists(L1,L2) == true); //Enpoint of L1 lies on L2
  REQUIRE(SpgMth::IntersectionExists(L2,L1) == true); //Enpoint of L1 lies on L2
  L1={{20,30},{50,20}}; L2={{50,30},{50,10}}; 
  REQUIRE(SpgMth::IntersectionExists(L1,L2) == true); //Enpoint of L1 lies on L2
  REQUIRE(SpgMth::IntersectionExists(L2,L1) == true); //Enpoint of L1 lies on L2
  L1={{20,30},{50,20}}; L2={{50,20},{60,30}}; 
  REQUIRE(SpgMth::IntersectionExists(L2,L1) == true); //Coinciding endpoints
  REQUIRE(SpgMth::IntersectionExists(L1,L2) == true); //Coinciding endpoints
  L1={{10,30},{20,45}}; L2={{20,40},{60,30}}; 
  REQUIRE(SpgMth::IntersectionExists(L2,L1) == false); //Segments don't cross FAILS!?
  REQUIRE(SpgMth::IntersectionExists(L1,L2) == false); //Segments don't cross FAILS!?

  
  }

  TEST_CASE( "Compute Angle, Subtended Angle 2d", "ComputeAngleInDegrees(), ComputeSubtendedAngleInDegrees()") 
  {
  auto percent = 0.001; 
  SpgMth::Point2d a{1,1}, b{3,1}, c{6,1};
  SpgMth::LineSeg2D s1{a,b}, s2{b,c};
  //horizontal, same dir segs
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(0, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(0, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(180, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(0, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(0, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(180, percent));

  //Right angles, positive
  a={1,1}; b={3,1}; c={3,4};  s1={a,b}, s2={b,c};
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(90, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(90, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(90, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(-90, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(-90, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(90, percent));

  //horizontal, opposite dir segs
  a={1,1}; b={3,1}; c={-3,1};  s1={a,b}, s2={b,c};
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(180, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(180, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(0, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(180, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(180, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(0, percent));

  //Right angles, negative
  a={1,1}; b={3,1}; c={3,-2};  s1={a,b}, s2={b,c};
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(-90, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(-90, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(90, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(90, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(90, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(90, percent));

  //Quadrant 1 angle
  a={1,1};b={4,2};c={9,5};  s1={a,b}, s2={b,c};
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(12.528808, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(12.528808, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(167.47119, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(-12.528808, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(-12.528808, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(167.47119, percent));

  //Angle, Quadrant 2
  a={1,1};b={4,2};c={-4,4};  s1={a,b}, s2={b,c};
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(147.5288, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(147.5288, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(32.471207, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(-147.5288, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(-147.5288, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(32.471207, percent));

  //Angle, Quadrant 3
  a={1,1};b={4,2};c={2,-2};  s1={a,b}, s2={b,c};
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(-135, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(-135, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(45, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(135, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(135, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(45, percent));

  //Angle, Quadrant 4
  a={1,1};b={4,2};c={11,1};  s1={a,b}, s2={b,c};
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(a,b,c),  CM::WithinRel(-26.565048, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s1,s2),  CM::WithinRel(-26.565048, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(a,b,c),  CM::WithinRel(153.43495, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(c,b,a),  CM::WithinRel(26.565048, percent));
  REQUIRE_THAT(SpgMth::ComputeAngleInDegrees(s2,s1),  CM::WithinRel(26.565048, percent));
  REQUIRE_THAT(SpgMth::ComputeSubtendedAngleInDegrees(c,b,a),  CM::WithinRel(153.43495, percent));

#if defined(RUN_BENCHMARKS)  
  BENCHMARK("Compute Angle 2d benchmark") { 
    return SpgMth::ComputeAngleInDegrees(a,b,c);
  };

  BENCHMARK("Compute Subtended Angle 2d benchmark") { 
    return SpgMth::ComputeSubtendedAngleInDegrees(a,b,c);
  };

#endif

  }

#if 0 //Todo - debug ssertion in DCEL code
  TEST_CASE( "Diagonal test", "DCEL::DiagonalCheck(Vertex*, Vertex*)" ) {
    std::vector<SpgMth::Point2d> poly_points =
    {
      {16.42f,12.51f},  //A 1
      {13.95,10.36},    //B 2
      {11.2,18.4},      //C 3
      {9.2,16.4},       //D 4
      {6.6,17.8},       //E 5
      {4,16},           //F 6
      {6.62,13.16},     //G 7
      {5.52,9.06},      //H 8
      {3.38,11.36},     //I 9
      {2.54,6.49},      //J 10
      {6.04,3.49},      //K 11
      {8.99,5.24},      //L 12
      {12,2},           //M 13
      {12.26,7.79},     //N 14
      {17.04,6.99}      //O 15
    };
    SpgMth::DCEL poly = SpgMth::DCEL(poly_points);
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(5), poly.GetVertex(5)).is_valid == false); //Same vertex!
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(7), poly.GetVertex(8)).is_valid == false); //Neighbours
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(8), poly.GetVertex(7)).is_valid == false); //Neighbours
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(1), poly.GetVertex(15)).is_valid == false); //Neighbours
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(15), poly.GetVertex(1)).is_valid == false); //Neighbours
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(9), poly.GetVertex(7)).is_valid == false);  //Exterior, 1 convex, 1 reflex 
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(7), poly.GetVertex(9)).is_valid == false);  //Exterior, 1 convex, 1 reflex 
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(1), poly.GetVertex(3)).is_valid == false);  //Exterior, 2 convex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(3), poly.GetVertex(1)).is_valid == false);  //Exterior, 2 convex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(12), poly.GetVertex(15)).is_valid == false);  //Intersection,  1 convex, 1 reflex 
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(15), poly.GetVertex(12)).is_valid == false);  //Intersection,  1 convex, 1 reflex 
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(9), poly.GetVertex(5)).is_valid == false);  //Intersection, 2 convex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(5), poly.GetVertex(9)).is_valid == false);  //Intersection, 2 convex

    REQUIRE(poly.DiagonalCheck(poly.GetVertex(2), poly.GetVertex(8)).is_valid == true);   //2 reflex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(8), poly.GetVertex(2)).is_valid == true);   //2 reflex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(7), poly.GetVertex(4)).is_valid == true);   //2 reflex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(4), poly.GetVertex(7)).is_valid == true);   //2 reflex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(10), poly.GetVertex(14)).is_valid == true);   //1 convex, 1 reflex 
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(14), poly.GetVertex(10)).is_valid == true);   //1 convex, 1 reflex 
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(11), poly.GetVertex(3)).is_valid == true);   //2 convex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(3), poly.GetVertex(11)).is_valid == true);   //2 convex
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(15), poly.GetVertex(2)).is_valid == true);   //1 convex, 1 reflex 
    REQUIRE(poly.DiagonalCheck(poly.GetVertex(2), poly.GetVertex(15)).is_valid == true);   //1 convex, 1 reflex 

  }
#endif

}

