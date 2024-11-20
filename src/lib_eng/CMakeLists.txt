set(ENG_SOURCES "eng.cpp" "eng_window.cpp" "eng_window.h")
set(ENG_HEADERS 
  "./include/eng/eng.h"
)
set(ENG_INCLUDES "./" "./core")

add_library(${LIB_ENG} STATIC ${ENG_SOURCES} ${ENG_HEADERS} )

add_subdirectory(core)

target_include_directories(${LIB_ENG} PRIVATE ${ENG_INCLUDES}) #only availably in this lib
target_include_directories(${LIB_ENG} PUBLIC "./include") # files included by apps

target_link_libraries(${LIB_ENG} PUBLIC OpenGL::GL)
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_GLAD} )
target_link_libraries(${LIB_ENG} PUBLIC glfw)
target_link_libraries(${LIB_ENG} PRIVATE glm::glm-header-only)
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_GEOM} ) #This also links to GLM
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_STB_IMG} )
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_IMGUI} ) # Also links to glfw