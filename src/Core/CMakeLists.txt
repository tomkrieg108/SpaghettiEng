add_library(${LIB_CORE} STATIC
)

target_sources(${LIB_CORE} PRIVATE 
  "./SmartPointerUtils.h"
  "./Logger.h"
  "./Logger.cpp"
  "./Core.h"
  "./LibCheck.cpp"
  "./LibCheck.h"
)

target_include_directories(${LIB_CORE}  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_CORE} PUBLIC 
  fmt::fmt
  spdlog::spdlog
)

target_link_libraries(${LIB_CORE} PUBLIC glm::glm-header-only) 

#For testing libs only
target_link_libraries(${LIB_CORE} PUBLIC 
  nlohmann_json::nlohmann_json  
  cxxopts::cxxopts
)

