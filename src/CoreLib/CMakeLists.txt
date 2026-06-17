add_library(${LIB_CORE} STATIC)

target_sources(${LIB_CORE} PRIVATE 
  "./PlatformDetect/ArchDetect.h"
  "./PlatformDetect/CompilerDetect.h"
  "./PlatformDetect/OSDetect.h"
  "./HelperMacros.h"
  "./SpgAssert.h"
  "./Logger.h"
  "./Logger.cpp"
  "./Core.h"
  "./Init.cpp"
  "./Init.h"
)

target_include_directories(${LIB_CORE}  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_CORE} PUBLIC
  fmt::fmt
  spdlog::spdlog
)

#For testing libs only
target_link_libraries(${LIB_CORE} PUBLIC
  nlohmann_json::nlohmann_json  
  cxxopts::cxxopts
)

