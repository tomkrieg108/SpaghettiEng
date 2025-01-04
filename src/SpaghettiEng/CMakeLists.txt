
add_library(${LIB_ENG} STATIC)

target_sources(${LIB_ENG} PRIVATE 
  "./Core/Application.cpp"
  "./Core/Window.cpp"
  "./Core/Log.cpp"
  "./Core/Input.cpp"

  "./Events/EventManager.cpp"

  "./ImGuiLayer/ImGuiLayer.cpp"

  "./OpenGL32/GL32Shader.cpp"
  )

target_sources(${LIB_ENG} PRIVATE
  "./Core/Base.h"
  "./Core/ArchitectureDetection.h"
  "./Core/Application.h"
  "./Core/Window.h"
  "./Core/Log.h"
  "./Core/Input.h"
  "./Core/KeyCodes.h"
  "./Core/MouseCodes.h"
  
  "./Events/Events.h"
  "./Events/EventManager.h"

  "./ImGuiLayer/ImGuiLayer.h"

  "./OpenGL32/GL32Shader.h"
)

target_sources(${LIB_ENG} PUBLIC 
  "./SpaghettiEng.h"
)

# changed from PRIVATE to PUBLIC - was getting compiler errors (not finding header files)
target_include_directories(${LIB_ENG}  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/Core"
  "${CMAKE_CURRENT_SOURCE_DIR}/Events"
  "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiLayer"
  "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL32"
) 

# Headers here are publicly exposed to other targets
target_include_directories(${LIB_ENG}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_link_libraries(${LIB_ENG} PUBLIC OpenGL::GL)
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_GLAD} )
target_link_libraries(${LIB_ENG} PUBLIC glfw)
target_link_libraries(${LIB_ENG} PUBLIC glm::glm-header-only)
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_GEOM} ) #This also links to GLM
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_STB_IMG} )
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_IMGUI} ) # Also links to glfw