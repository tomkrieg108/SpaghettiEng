add_library(${LIB_GEOM} STATIC 
  "./Geom.cpp"
  "./Include/Geom/Geom.h"
)

target_include_directories(${LIB_GEOM} PRIVATE 
  "./"
)

target_include_directories(${LIB_GEOM} PUBLIC 
  "./Include"
  "./Include/Geom"
)

target_link_libraries(${LIB_GEOM} PUBLIC 
  nlohmann_json::nlohmann_json  
  fmt::fmt
  spdlog::spdlog
  cxxopts::cxxopts
)

target_link_libraries(${LIB_GEOM} PUBLIC glm::glm-header-only) 