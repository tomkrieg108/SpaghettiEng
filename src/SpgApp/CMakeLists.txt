add_executable(${APP_SPG} ${EDITOR_SOURCES} ${EDITOR_HEADERS})

target_sources(${APP_SPG} PRIVATE
  "./SpgApp.h"
  "./SpgApp.cpp"
)

target_include_directories(${LIB_ENG}  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries( ${APP_SPG} PUBLIC ${LIB_ENG})