
add_library(${LIB_ENG} STATIC)

target_sources(${LIB_ENG} PRIVATE 

  "./Core/Application.cpp"
  "./Core/AppLayer.cpp"
  "./Core/Window.cpp"
  "./Core/InputState.cpp"
  "./Core/Layer.cpp"

  "./Events/EventManager.cpp"

  "./ImGuiUtils/ImGuiUtils.cpp"

  "./Render/Camera/Camera.cpp"
  "./Render/Camera/Camera2D.cpp"
  "./Render/Camera/CameraController2D.cpp"
  "./Render/Renderer.cpp"
  "./Render/Mesh/MeshData.cpp"
  "./Render/Backends/OpenGL/GLShader.cpp"
  "./Render/Backends/OpenGL/GLShaderUtils.cpp"
  "./Render/Backends/OpenGL/GLContext.cpp"
  "./Render/Backends/OpenGL/GLBuffer.cpp"
  "./Render/Backends/OpenGL/GLVertexArray.cpp"
  "./Render/Backends/OpenGL/GLRenderer.cpp"
 
  "./Scene/Scene.cpp"
  "./Scene/Entity.cpp"
  "./Scene/SceneCameraController.cpp"
  )

if(WIN32)
  target_sources(${LIB_ENG} PRIVATE 
  "./Render/Backends/OpenGL/GLTextRenderer.cpp"
  )
endif()

target_sources(${LIB_ENG} PRIVATE
  "./Core/Application.h"
  "./Core/AppLayer.h"
  "./Core/ServiceLocator.h"
  "./Core/Window.h"
  "./Core/InputState.h"
  "./Core/Layer.h"
  "./Core/KeyCodes.h"
  "./Core/MouseCodes.h"
  "./Events/Events.h"
  "./Events/EventManager.h"
  "./ImGuiUtils/ImGuiUtils.h"

  "./Render/Camera/Camera.h"
  "./Render/Camera/Camera2D.h"
  "./Render/Camera/CameraController2D.h"
  "./Render/Renderer.h"
  "./Render/Mesh/MeshData.h"
  "./Render/Backends/OpenGL/GLShader.h"
  "./Render/Backends/OpenGL/GLShaderUtils.h"
  "./Render/Backends/OpenGL/GLContext.h"
  "./Render/Backends/OpenGL/GLBuffer.h"
  "./Render/Backends/OpenGL/GLVertexArray.h"
  "./Render/Backends/OpenGL/GLBufferLayout.h"
  "./Render/Backends/OpenGL/GLRenderer.h"
  "./Render/Backends/OpenGL/GLTextRenderer.h"

  "./Scene/Components.h"
  "./Scene/Entity.h"
  "./Scene/Registry.h"
  "./Scene/Scene.h"
  "./Scene/SceneCameraController.h"
)

if(WIN32)
  target_sources(${LIB_ENG} PRIVATE 
  "./Render/Backends/OpenGL/GLTextRenderer.h"
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
  "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL"
) 

target_link_libraries(${LIB_ENG} PUBLIC OpenGL::GL)
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_GLAD} )
target_link_libraries(${LIB_ENG} PUBLIC glfw)
target_link_libraries(${LIB_ENG} PUBLIC glm::glm-header-only)
target_link_libraries(${LIB_ENG} PUBLIC EnTT::EnTT)
target_link_libraries(${LIB_ENG} PUBLIC freetype)
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_CORE} )   
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_MATH} )    
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_GEOM} )     
target_link_libraries(${LIB_ENG} PUBLIC ${LIB_CYCLONE} )  
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_STB_IMG} )
target_link_libraries(${LIB_ENG} PUBLIC ${EXT_IMGUI} )

#For testing libs only
target_link_libraries(${LIB_ENG} PUBLIC
  nlohmann_json::nlohmann_json  
  cxxopts::cxxopts
)