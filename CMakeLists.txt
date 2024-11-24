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
set(LIB_ENG "lib_eng")
set(LIB_GEOM "lib_geom")
set(APP_EDITOR "app_editor")

add_subdirectory(src) 
add_subdirectory(external)   

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
