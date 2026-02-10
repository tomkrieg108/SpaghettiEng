add_library(${LIB_MATH} STATIC 
  "./FloatingPoint.h"
  "./MathX.h"
  "./MathX.cpp"
)

target_include_directories(${LIB_MATH} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_MATH} PUBLIC ${LIB_COMMON} )
target_link_libraries(${LIB_MATH} PUBLIC glm::glm-header-only) 