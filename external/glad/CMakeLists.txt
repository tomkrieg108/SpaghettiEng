
add_library(${EXT_GLAD} STATIC 
  "src/gl.c" 
  "include/glad/gl.h" 
  "include/KHR/khrplatform.h"
)

target_include_directories(${EXT_GLAD} PUBLIC
  "./include"
)

target_link_libraries(${EXT_GLAD} ) 


