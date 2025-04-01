add_library(${LIB_COMMON} STATIC
)

target_sources(${LIB_COMMON} PRIVATE 
  "./SmartPointerUtils.h"
  "./Logger.h"
  "./Logger.cpp"
  "./Common.h"
  "./UUID.cpp"
  "./UUID.h"
  "./LibCheck.cpp"
  "./LibCheck.h"
)

target_include_directories(${LIB_COMMON}  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_COMMON} PUBLIC 
  spdlog::spdlog
)

target_link_libraries(${LIB_COMMON} PUBLIC glm::glm-header-only) 

#For testing libs only
target_link_libraries(${LIB_COMMON} PUBLIC 
  nlohmann_json::nlohmann_json  
  fmt::fmt
  cxxopts::cxxopts
)

