cmake_minimum_required(VERSION 3.23)

project(
    "BubbleSoup"
    VERSION 1.0.0
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#ChatGPT
# Ensure CPM.cmake is available
#[[
if (NOT CPM_SOURCE_CACHE)
  set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/cpm-cache")
endif()
# Download CPM.cmake if it doesn't exist
if (NOT EXISTS "${CPM_SOURCE_CACHE}/CPM.cmake")
  file(DOWNLOAD
    "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake"
    "${CPM_SOURCE_CACHE}/CPM.cmake"
    TLS_VERIFY ON)
endif()
include("${CPM_SOURCE_CACHE}/CPM.cmake")
]]

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

#CPM - lib needs to be github / gitlab cmake projects to use
message(STATUS "Using CPM package manager")
include(CPM) #looks in cmake dir
cpmaddpackage("gh:nlohmann/json#v3.11.3") # gh: look in github
cpmaddpackage("gh:fmtlib/fmt#9.1.0")
cpmaddpackage("gh:gabime/spdlog#v1.13.0")
cpmaddpackage("gh:jarro2783/cxxopts#v3.1.1")

cpmaddpackage(
  NAME glfw
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.4
  # OPTIONS "GLFW_BUILD_SHARED_LIBS OFF"  # Build as a static library (does by default anyway)
  OPTIONS "GLFW_BUILD_SHARED_LIBS OFF" "GLFW_BUILD_WAYLAND OFF"
  # For linux, need this: OPTIONS "GLFW_BUILD_SHARED_LIBS OFF" "GLFW_BUILD_WAYLAND OFF"  
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
set(LIB_ENG "lib_eng")
set(LIB_GEOM "lib_geom")
set(APP_EDITOR "app_editor")
set(APP_SANDBOX "app_sandbox")

#[[
message(STATUS "Project Name:" ${PROJECT_NAME})
message(STATUS "Editor Name:" ${APP_EDITOR})
message(STATUS "Eng Lib Name:" ${LIB_ENG})
message(STATUS "Geom Lib Name:" ${LIB_GEOM})
]]

add_subdirectory(src) 
add_subdirectory(external)              
