add_library(${LIB_GEOM} STATIC 
  "Geom.cpp"
  "Geom.h"
)

target_include_directories(${LIB_GEOM} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_GEOM} PUBLIC 
  nlohmann_json::nlohmann_json  
  fmt::fmt
  spdlog::spdlog
  cxxopts::cxxopts
)

target_link_libraries(${LIB_GEOM} PUBLIC glm::glm-header-only) 