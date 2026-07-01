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
)

target_include_directories(${LIB_CORE}  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_CORE} PUBLIC
  fmt::fmt
  spdlog::spdlog
)



