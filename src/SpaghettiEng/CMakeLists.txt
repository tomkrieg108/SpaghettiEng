
add_library(${LIB_ENG} STATIC)

target_sources(${LIB_ENG} PRIVATE 
  "./Core/Application.cpp"
  "./Core/Window.cpp"
  "./Core/Log.cpp"
  )

target_sources(${LIB_ENG} PRIVATE
  "./Core/Base.h"
  "./Core/ArchitectureDetection.h"
  "./Core/Application.h"
  "./Core/Window.h"
  "./Core/Log.h"
  "./Events/Events.h"
  "./Events/EventManager.h"
)

target_sources(${LIB_ENG} PUBLIC 
  "./Include/SpaghettiEng/SpaghettiEng.h"
)

# Headers here are not publicly exposed
target_include_directories(${LIB_ENG}  PRIVATE 
  "./"
  "./Core"
  "./Events"
) 

# Headers here are publicly exposed to other targets
target_include_directories(${LIB_ENG}  PUBLIC 
  "./Include"
  "./Include/SpaghettiEng"
) 

target_link_libraries(${LIB_ENG} PUBLIC OpenGL::GL)
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_GLAD} )
target_link_libraries(${LIB_ENG} PUBLIC glfw)
target_link_libraries(${LIB_ENG} PUBLIC glm::glm-header-only)
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_GEOM} ) #This also links to GLM
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_STB_IMG} )
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_IMGUI} ) # Also links to glfw