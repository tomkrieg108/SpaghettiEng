
add_library(${LIB_ENG} STATIC)

target_sources(${LIB_ENG} PRIVATE 
  "./Core/Application.cpp"
  "./Core/AppLayer.cpp"
  "./Core/Window.cpp"
  "./Core/Input.cpp"
  "./Core/Layer.cpp"
  "./Events/EventManager.cpp"
  "./ImGuiUtils/ImGuiUtils.cpp"
  "./OpenGL32/GL32Shader.cpp"
  "./OpenGL32/GL32ShaderUtils.cpp"
  "./OpenGL32/GL32Context.cpp"
  "./OpenGL32/GL32Buffer.cpp"
  "./OpenGL32/GL32VertexArray.cpp"
  "./OpenGL32/GL32Renderer.cpp"
  "./Camera/Camera2D.cpp"
  "./Camera/CameraController2D.cpp"
  )

if(WIN32)
  target_sources(${LIB_ENG} PRIVATE 
  "./OpenGL32/GL32TextRenderer.cpp"
  )
endif()

target_sources(${LIB_ENG} PRIVATE
  "./Core/Application.h"
  "./Core/AppLayer.h"
  "./Core/AppContext.h"
  "./Core/Window.h"
  "./Core/Input.h"
  "./Core/Layer.h"
  "./Core/KeyCodes.h"
  "./Core/MouseCodes.h"
  "./Events/Events.h"
  "./Events/EventManager.h"
  "./ImGuiUtils/ImGuiUtils.h"
  "./OpenGL32/GL32Shader.h"
  "./OpenGL32/GL32ShaderUtils.h"
  "./OpenGL32/GL32Context.h"
  "./OpenGL32/GL32Buffer.h"
  "./OpenGL32/GL32VertexArray.h"
  "./OpenGL32/GL32BufferLayout.h"
  "./OpenGL32/GL32Renderer.h"
  "./Camera/Camera2D.h"
  "./Camera/CameraController2D.h"
)

if(WIN32)
  target_sources(${LIB_ENG} PRIVATE 
  "./OpenGL32/GL32TextRenderer.h"
  )
endif()

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

target_link_libraries(${LIB_ENG} PUBLIC OpenGL::GL)
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_GLAD} )
target_link_libraries(${LIB_ENG} PUBLIC glfw)
target_link_libraries(${LIB_ENG} PUBLIC glm::glm-header-only)
target_link_libraries(${LIB_ENG} PUBLIC EnTT::EnTT)
target_link_libraries(${LIB_ENG} PUBLIC freetype)
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_COMMON} )   #links to spdlog, GLM, json, fmt, cxxopts
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_MATH} )     #Also links to GLM & Common
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_GEOM} )     #Also links to GLM & Common
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_CYCLONE} )  #Also links to GLM & Common
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_STB_IMG} )
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_IMGUI} )    #Also links to glfw