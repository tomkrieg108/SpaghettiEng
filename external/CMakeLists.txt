add_subdirectory(glad)
add_subdirectory(stb_img)
#add_subdirectory(imgui)

#imgui (git submodule)
add_library(${EXT_IMGUI} STATIC
  "./imgui/imconfig.h"
  "./imgui/imgui.cpp"
  "./imgui/imgui.h"
  "./imgui/imgui_demo.cpp"
  "./imgui/imgui_draw.cpp"
  "./imgui/imgui_internal.h"
  "./imgui/imgui_tables.cpp"
  "./imgui/imgui_widgets.cpp"
  "./imgui/imstb_rectpack.h"
  "./imgui/imstb_textedit.h"
  "./imgui/imstb_truetype.h"
  "./imgui/backends/imgui_impl_opengl3.cpp"
  "./imgui/backends/imgui_impl_opengl3.h"
  "./imgui/backends/imgui_impl_glfw.cpp"
  "./imgui/backends/imgui_impl_glfw.h"
)

target_include_directories(${EXT_IMGUI} PUBLIC
  "./imgui"
  "./imgui//backends"
)

#need to link against glfw so that imgui picks up the include directies for glfw
# This should automatically add the GLFW include directories to imgui's build context because CMake propagates the GLFW include paths when you link against GLFW::GLFW.
target_link_libraries(${EXT_IMGUI} PUBLIC glfw)
