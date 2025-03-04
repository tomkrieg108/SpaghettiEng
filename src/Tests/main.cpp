#include <Geometry/Geometry.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

namespace GeomTest 
{
  /*
  TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Geom::Factorial( 0) == 1 );
    REQUIRE( Geom::Factorial( 1) == 1 );
    REQUIRE( Geom::Factorial( 2) == 2 );
    REQUIRE( Geom::Factorial( 3) == 6 );
    REQUIRE( Geom::Factorial(10) == 3'628'800 );
  }
  */

  namespace CM = Catch::Matchers;

  
  TEST_CASE( "SignedArea", "[ComputeSignedArea]" ) {
    Geom::Point2d A{-2.96,-1.48}, B{5.044,1.43}, C{0.86,1.36};
    auto expected = 5.80758; auto percent = 0.001; const auto epsilon = Geom::Epsilon(100);
    REQUIRE_THAT( Geom::ComputeSignedArea(A,B,C), CM::WithinRel(expected, percent) || CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( Geom::ComputeSignedArea(B,C,A), CM::WithinRel(expected, percent) || CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( Geom::ComputeSignedArea(C,A,B), CM::WithinRel(expected, percent) || CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( Geom::ComputeSignedArea(A,C,B), CM::WithinRel(-expected, percent) || CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( Geom::ComputeSignedArea(B,A,C), CM::WithinRel(-expected, percent) || CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( Geom::ComputeSignedArea(C,B,A), CM::WithinRel(-expected, percent) || CM::WithinAbs(-expected, epsilon) );  //CW

    //near zero area
    C = {-1.556, -0.946}; expected = 0.09425; percent = 0.001;  //percent diff needs to be larger for values closer to zero. Not abs diff
    REQUIRE_THAT( Geom::ComputeSignedArea(A,B,C), CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( Geom::ComputeSignedArea(B,C,A), CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( Geom::ComputeSignedArea(C,A,B), CM::WithinAbs(expected, epsilon) );  //CCW
    REQUIRE_THAT( Geom::ComputeSignedArea(A,C,B), CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( Geom::ComputeSignedArea(B,A,C), CM::WithinAbs(-expected, epsilon) );  //CW
    REQUIRE_THAT( Geom::ComputeSignedArea(C,B,A), CM::WithinAbs(-expected, epsilon) );  //CW

   //zero area (colinear)
    A={-6,2}, B={-1.622,1.276}, C={-4.60752,1.76972};  expected = 0; percent = 0.001;
    REQUIRE_THAT( Geom::ComputeSignedArea(A,B,C),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( Geom::ComputeSignedArea(B,C,A),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( Geom::ComputeSignedArea(C,A,B),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( Geom::ComputeSignedArea(A,C,B),  CM::WithinAbs(-expected, epsilon) );  
    REQUIRE_THAT( Geom::ComputeSignedArea(B,A,C),  CM::WithinAbs(-expected, epsilon) );  
    REQUIRE_THAT( Geom::ComputeSignedArea(C,B,A),  CM::WithinAbs(-expected, epsilon) );  

    //coincident point(s)
    expected = 0; percent = 0.001;
    REQUIRE_THAT( Geom::ComputeSignedArea(A,A,B),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( Geom::ComputeSignedArea(A,B,B),  CM::WithinAbs(expected, epsilon) );  
    REQUIRE_THAT( Geom::ComputeSignedArea(A,A,A),  CM::WithinAbs(expected, epsilon) );  
  }

  TEST_CASE( "Orientation2d", "[Orientation2d]" ) {
    Geom::Point2d A{-2.96,-1.48}, B{5.044,1.43}, C{-3.02,0.924};
    REQUIRE(Geom::Orientation2d(A,B,C) == Geom::RelativePos::Left);
    REQUIRE(Geom::Orientation2d(B,C,A) == Geom::RelativePos::Left);
    REQUIRE(Geom::Orientation2d(C,A,B) == Geom::RelativePos::Left);
    REQUIRE(Geom::Orientation2d(A,C,B) == Geom::RelativePos::Right);
    REQUIRE(Geom::Orientation2d(C,B,A) == Geom::RelativePos::Right);
    REQUIRE(Geom::Orientation2d(B,A,C) == Geom::RelativePos::Right);

    BENCHMARK("Geom::Orientation2d - left") {
        return Geom::Orientation2d(A,B,C);
    };

    //Near Colinear
    C={-0.918,-0.704};
    REQUIRE(Geom::Orientation2d(A,B,C) == Geom::RelativePos::Left);
    REQUIRE(Geom::Orientation2d(B,C,A) == Geom::RelativePos::Left);
    REQUIRE(Geom::Orientation2d(C,A,B) == Geom::RelativePos::Left);
    REQUIRE(Geom::Orientation2d(A,C,B) == Geom::RelativePos::Right);
    REQUIRE(Geom::Orientation2d(C,B,A) == Geom::RelativePos::Right);
    REQUIRE(Geom::Orientation2d(B,A,C) == Geom::RelativePos::Right);

    //coincident endpoints
    REQUIRE(Geom::Orientation2d(A,B,A) == Geom::RelativePos::Origin);
    REQUIRE(Geom::Orientation2d(A,B,B) == Geom::RelativePos::Destination);

    //Colinear case - negative slope for line seg
    A={-6,2}, B={-2.018,1.232}, C={-3.93443,1.60162}; //C is between a and B (colinear and interior)
    REQUIRE(Geom::Orientation2d(A,B,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(B,A,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(A,C,B) == Geom::RelativePos::Beyond);
    REQUIRE(Geom::Orientation2d(C,B,A) == Geom::RelativePos::Behind);

    //Colinear case - positive slope for line seg
    A={-6,1}, B={-2.19875,2.33779}, C={-3.89479,1.7409}; //C is between a and B (colinear and interior)
    REQUIRE(Geom::Orientation2d(A,B,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(B,A,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(A,C,B) == Geom::RelativePos::Beyond);
    REQUIRE(Geom::Orientation2d(C,B,A) == Geom::RelativePos::Behind);

    //Horizontal line
    A={-6,1}, B={-2,1}, C={-4.66632,1}; //C is between a and B (colinear and interior)
    REQUIRE(Geom::Orientation2d(A,B,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(B,A,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(A,C,B) == Geom::RelativePos::Beyond);
    REQUIRE(Geom::Orientation2d(C,B,A) == Geom::RelativePos::Behind);

    //Vertical line
     A={-2,3}, B={-2,1}, C={-2,2.03742}; //C is between a and B (colinear and interior)
    REQUIRE(Geom::Orientation2d(A,B,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(B,A,C) == Geom::RelativePos::Between);
    REQUIRE(Geom::Orientation2d(A,C,B) == Geom::RelativePos::Beyond);
    REQUIRE(Geom::Orientation2d(C,B,A) == Geom::RelativePos::Behind);

     BENCHMARK("Geom::Orientation2d - between") {
        return Geom::Orientation2d(A,B,C);
    };
    
  }
}

