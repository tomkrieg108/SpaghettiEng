
#--------------------------------------------------------------
# Glad
#--------------------------------------------------------------
add_library(${EXT_GLAD} STATIC 
  "./glad/src/gl.c" 
  "./glad/include/glad/gl.h" 
  "./glad/include/KHR/khrplatform.h"
)
target_include_directories(${EXT_GLAD} PUBLIC
  "./glad/include"
)
target_link_libraries(${EXT_GLAD} ) 


#--------------------------------------------------------------
# stb_image
#--------------------------------------------------------------
add_library(${EXT_STB_IMG} STATIC 
  "./stb_imgage/stb_image.cpp"
  "./stb_imgage/stb_image.h"
)
target_include_directories(${EXT_STB_IMG} PUBLIC
  "./stb_imgage"
  "."
)
target_link_libraries(${EXT_STB_IMG}) 


#--------------------------------------------------------------
#imgui - docking branch (git submodule)
#--------------------------------------------------------------
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
  "./imgui/backends"
  "./"
)

#[[
Need to link against glfw so that imgui picks up the include directories for glfw
CMake propagates the GLFW include paths when you link against glfw, so this should automatically add the GLFW include directories to imgui's build context. 
]]
target_link_libraries(${EXT_IMGUI} PUBLIC glfw)
