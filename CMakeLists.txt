cmake_minimum_required(VERSION 3.23)

project(
    "SpaghettiEng"
    VERSION 1.0.0
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#include() looks in cmake dir 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

#CPM - lib needs to be github / gitlab cmake projects to use
message(STATUS "Using CPM package manager")
include(CPM) 
cpmaddpackage("gh:nlohmann/json#v3.11.3") # gh: look in github
cpmaddpackage("gh:fmtlib/fmt#11.0.2")
cpmaddpackage("gh:gabime/spdlog#v1.15.0")
cpmaddpackage("gh:jarro2783/cxxopts#v3.2.0")

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

find_package(OpenGL REQUIRED) # Add OpenGL (usually available system-wide)

set(EXT_GLAD "lib_glad")
set(EXT_STB_IMG "lib_stb_image")
set(EXT_IMGUI "lib_imgui_docking")
set(LIB_COMMON "lib_common")
set(LIB_ENG "lib_eng")
set(LIB_GEOM "lib_geom")
set(APP_EDITOR "app_editor")

#processed depth-first
add_subdirectory(src) 
add_subdirectory(external)   

# Build type - preprocessor macro is compiler dependent, so set here 
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
target_compile_definitions(${APP_EDITOR} PRIVATE 
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
    target_compile_definitions(${APP_EDITOR} PRIVATE SPG_WIN64)
  else()
    message(FATAL_ERROR "Cmake Msg: Unsupported platform detected: Windows 32-bit")
  endif()
elseif((UNIX AND NOT APPLE))
  message(STATUS "Cmake Msg: Platform is Linux")
  target_compile_definitions(${LIB_COMMON} PRIVATE SPG_LINUX)
  target_compile_definitions(${LIB_GEOM} PRIVATE SPG_LINUX)
  target_compile_definitions(${LIB_ENG} PRIVATE SPG_LINUX)
  target_compile_definitions(${APP_EDITOR} PRIVATE SPG_LINUX)
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
  #<list>
  <unordered_map>
  #<unordered_set>
  #<map>
  <memory>
  #<functional>
  <random>
  #<utility>
  <algorithm>
  <variant>
  <queue>
  <type_traits>
  <numbers>
)


