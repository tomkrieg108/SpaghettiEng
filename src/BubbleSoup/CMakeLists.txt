set(EDITOR_SOURCES "BubbleSoup.cpp")
set(EDITOR_HEADERS "BubbleSoup.h")
set(EDITOR_INCLUDES ".")

add_executable(${APP} ${EDITOR_SOURCES} ${EDITOR_HEADERS})
target_include_directories(${APP} PUBLIC ${EDITOR_INCLUDES})
target_link_libraries( ${APP} PUBLIC ${LIB_ENG})