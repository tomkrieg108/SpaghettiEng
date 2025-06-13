set(EDITOR_SOURCES 
  "./GeomApp.cpp" 
  "./DefaultLayer.cpp" 
  "./DefaultLayerUI.cpp")

set(EDITOR_HEADERS 
  "./GeomApp.h" 
  "./DefaultLayer.h")

set(EDITOR_INCLUDES ".")

add_executable(${APP_GEOM} ${EDITOR_SOURCES} ${EDITOR_HEADERS})
target_include_directories(${APP_GEOM} PUBLIC ${EDITOR_INCLUDES})
target_link_libraries( ${APP_GEOM} PUBLIC ${LIB_ENG})