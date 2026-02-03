cmake_minimum_required(VERSION 3.23)

project(
    "SpaghettiEng"
    VERSION 1.0.0
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_TESTING "Enable a Unit Testing Build" ON)

#include() looks in cmake dir 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

#CPM - lib needs to be github / gitlab cmake projects to use
#todo CPM can be brought in automatically via 
# include(FetchContent)
# FetchContent_Declare(
#   CPM
#   GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake.git
#   GIT_TAG v0.38.2 # Use the latest version
# )
# FetchContent_MakeAvailable(CPM)

message(STATUS "Using CPM package manager")
include(CPM) 
cpmaddpackage("gh:nlohmann/json#v3.11.3") # gh: look in github
cpmaddpackage("gh:fmtlib/fmt#11.0.2")
cpmaddpackage("gh:gabime/spdlog#v1.15.0")
cpmaddpackage("gh:jarro2783/cxxopts#v3.2.0")
cpmaddpackage("gh:skypjack/entt#v3.15.0")
# set(SPDLOG_FMT_EXTERNAL ON) # Force spdlog to use external fmt

cpmaddpackage(
  NAME glfw
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.4
  OPTIONS "GLFW_BUILD_SHARED_LIBS OFF" "GLFW_BUILD_WAYLAND OFF"
  # For linux, need "GLFW_BUILD_WAYLAND OFF" for 3.4 (but not for 3.3.8), 
  # alternatively, need to install lib for wayland windowing support
)

cpmaddpackage(
  NAME glm
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG 1.0.1
)


# Add dependencies before FreeType
# CPMAddPackage(
#     NAME zlib
#     GITHUB_REPOSITORY madler/zlib
#     GIT_TAG v1.3.1  # Replace with the latest version
# )

#  CPMAddPackage(
#     NAME libpng
#     GITHUB_REPOSITORY glennrp/libpng
#     GIT_TAG v1.6.37  # Replace with the latest version
# )

#  CPMAddPackage(
#     NAME bzip2
#     GITHUB_REPOSITORY philr/bzip2
#     GIT_TAG v1.0.8
# )

#  CPMAddPackage(
#     NAME brotli
#     GITHUB_REPOSITORY google/brotli
#     GIT_TAG v1.1.0
# )

set(FT_WITH_ZLIB OFF CACHE BOOL "Disable FreeType Zlib support")
set(FT_WITH_PNG OFF CACHE BOOL "Disable FreeType PNG support")
set(FT_WITH_BZIP2 OFF CACHE BOOL "Disable FreeType BZip2 support")
set(FT_WITH_BROTLI OFF CACHE BOOL "Disable FreeType Brotli support")
set(FT_WITH_HARFBUZZ OFF CACHE BOOL "Disable FreeType HarfBuzz support")

#Add FreeType
if(WIN32)
  CPMAddPackage(
      NAME freetype
      GITHUB_REPOSITORY freetype/freetype
      GIT_TAG VER-2-13-3
      OPTIONS
          "CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=ON"  # Disable advanced text shaping
          "CMAKE_DISABLE_FIND_PACKAGE_BZip2=ON"     # Disable BZip2 support
          "CMAKE_DISABLE_FIND_PACKAGE_BrotliDec=ON" # Disable Brotli support
          "CMAKE_DISABLE_FIND_PACKAGE_PNG=ON"       # Disable PNG support
          "CMAKE_DISABLE_FIND_PACKAGE_ZLIB=ON"      # Disable Zlib support
  )
endif()

if(ENABLE_TESTING)
  cpmaddpackage(
    NAME catch2
    GITHUB_REPOSITORY catchorg/catch2
    GIT_TAG v3.8.0
  )
endif()


find_package(OpenGL REQUIRED) # Add OpenGL (usually available system-wide)

#Define variables for all targets
set(EXT_GLAD "lib_glad")
set(EXT_STB_IMG "lib_stb_image")
set(EXT_IMGUI "lib_imgui_docking")
set(LIB_COMMON "lib_common")
set(LIB_MATH "lib_math")
set(LIB_GEOM "lib_geom")
set(LIB_PHYS "lib_phys")
set(LIB_ENG "lib_eng")
set(APP_GEOM "geom_app")

if(ENABLE_TESTING)
  set(TEST_GEOM "test_geom")
  # include(CTest)
  # enable_testing()
endif()

#processe depth-first
add_subdirectory(external)  
add_subdirectory(src) 

# Build type (Debug, Release etc) - preprocessor macro is compiler dependent, so set here 
# Need to use generator expression for multi-config generators (MSVC)
target_compile_definitions(${LIB_COMMON} PRIVATE 
  $<$<CONFIG:Debug>:SPG_DEBUG>
  $<$<CONFIG:Release>:SPG_RELEASE>
)
target_compile_definitions(${LIB_GEOM} PRIVATE 
  $<$<CONFIG:Debug>:SPG_DEBUG>
  $<$<CONFIG:Release>:SPG_RELEASE>
)
target_compile_definitions(${LIB_ENG} PRIVATE 
  $<$<CONFIG:Debug>:SPG_DEBUG>
  $<$<CONFIG:Release>:SPG_RELEASE>
)
target_compile_definitions(${APP_GEOM} PRIVATE 
  $<$<CONFIG:Debug>:SPG_DEBUG>
  $<$<CONFIG:Release>:SPG_RELEASE>
)

#platform detection.  This can also be done in code (#defines consistent between compilers), but this allows for linking in different libs depending on platform
if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Cmake Msg: Platform is Windows 64-bit")
    target_compile_definitions(${LIB_COMMON} PRIVATE SPG_WIN64)
    target_compile_definitions(${LIB_GEOM} PRIVATE SPG_WIN64)
    target_compile_definitions(${LIB_ENG} PRIVATE SPG_WIN64)
    target_compile_definitions(${APP_GEOM} PRIVATE SPG_WIN64)
  else()
    message(FATAL_ERROR "Cmake Msg: Unsupported platform detected: Windows 32-bit")
  endif()
elseif((UNIX AND NOT APPLE))
  message(STATUS "Cmake Msg: Platform is Linux")
  target_compile_definitions(${LIB_COMMON} PRIVATE SPG_LINUX)
  target_compile_definitions(${LIB_GEOM} PRIVATE SPG_LINUX)
  target_compile_definitions(${LIB_ENG} PRIVATE SPG_LINUX)
  target_compile_definitions(${APP_GEOM} PRIVATE SPG_LINUX)
elseif(APPLE)  
  message(FATAL_ERROR "Cmake Msg: Unsupported platform detected: Apple")
else()
  message(FATAL_ERROR "Cmake Msg: Unsupported platform detected: Unknown platform")
endif()

#compiler detection - can set compiler flags accordingly
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   message(STATUS "Cmake Msg: Compiler is MSVC")
  #target_compile_options(myproject PRIVATE /W4)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(STATUS "Cmake Msg: Compiler is GNU")
  #target_compile_options(myproject PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")  
  message(STATUS "Cmake Msg: Compiler is Clang") 
  #target_compile_options(myproject PRIVATE -Wall -Wextra)
endif()

#architecture detection - these variables are compiler independent supposedly
# but CMAKE_SYSTEM_PROCESSOR variable not set for windows
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  message(STATUS "Cmake Msg: Architecture: x86_64")
  #target_compile_definitions(my_target PRIVATE ARCH_X86_64)
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "M_X64")
  message(STATUS "Cmake Msg: Architecture: M_X64")  
  #target_compile_definitions(my_target PRIVATE ARCH_X86_64)
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
  message(STATUS "Cmake Msg: Architecture: ARM64")
  #target_compile_definitions(my_target PRIVATE ARCH_ARM64)
else()
  message(STATUS "Cmake Msg: Cmake Detected Architecture:")
  message(STATUS CMAKE_SYSTEM_PROCESSOR) 
endif()



target_precompile_headers(${LIB_COMMON} PRIVATE 
  <cstdint>
  <cmath>
  <string>
  <string_view>
  <vector>
  <random>
  <algorithm>
  <numbers>
  <random>
  <memory>
)

target_precompile_headers(${LIB_GEOM} PRIVATE 
  <cstdint>
  <cmath>
  <string>
  <string_view>
  <vector>
  <map>
  <unordered_map>
  <set>
  <random>
  <algorithm>
  <numbers>
)

target_precompile_headers(${LIB_ENG} PRIVATE 
  <cstdint>
  <cmath>
  <iostream>
  <fstream>
  <string>
  <string_view>
  <sstream>
  <vector>
  <array>
  <unordered_map>
  <map>
  <memory>
  <random>
  <algorithm>
  <variant>
  <queue>
  <type_traits>
  <numbers>
)

target_precompile_headers(${APP_GEOM} PRIVATE 
  <cstdint>
  <cmath>
  <iostream>
  <fstream>
  <string>
  <string_view>
  <sstream>
  <vector>
  <array>
  <unordered_map>
  <memory>
  <random>
  <algorithm>
  <variant>
  <queue>
  <type_traits>
  <numbers>
)


