add_library(${LIB_GEOM} STATIC 
  "Geometry.cpp"
  "Geometry.h"
)

target_include_directories(${LIB_GEOM} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_GEOM} PUBLIC ${LIB_COMMON} )
target_link_libraries(${LIB_GEOM} PUBLIC glm::glm-header-only) 