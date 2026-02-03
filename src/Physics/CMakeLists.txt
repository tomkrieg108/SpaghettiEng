add_library(${LIB_PHYS} STATIC 
  "./Core.h"
  "./Core.cpp"
)

target_include_directories(${LIB_PHYS} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_PHYS} PUBLIC ${LIB_COMMON} )
target_link_libraries(${LIB_PHYS} PUBLIC glm::glm-header-only) 