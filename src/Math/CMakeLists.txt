
add_library(${LIB_MATH} STATIC 
  "./FloatingPoint.h"
  "./MathX.h"
  "./MathX.cpp"

  "./AML/AML.h"
  "./AML/AMLVector3.h"
  "./AML/AMLVector3.cpp"
  "./AML/AMLMatrix33.h"
  "./AML/AMLMatrix33.cpp"
)

target_include_directories(${LIB_MATH} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_MATH} PUBLIC ${LIB_COMMON} )
target_link_libraries(${LIB_MATH} PUBLIC glm::glm-header-only) 