add_library(${EXT_STB_IMG} STATIC 
  "./stb_image.cpp"
  "./stb_image.h"
)


target_include_directories(${EXT_STB_IMG} PUBLIC
  "."
)


target_link_libraries(${EXT_STB_IMG}) 
