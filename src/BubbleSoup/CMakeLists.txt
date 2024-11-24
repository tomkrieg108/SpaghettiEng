set(EDITOR_SOURCES "BubbleSoup.cpp")
set(EDITOR_HEADERS "BubbleSoup.h")
set(EDITOR_INCLUDES "./")

add_executable(${APP_EDITOR} ${EDITOR_SOURCES} ${EDITOR_HEADERS})
target_include_directories(${APP_EDITOR} PUBLIC ${EDITOR_INCLUDES})
target_link_libraries( ${APP_EDITOR} PUBLIC ${LIB_ENG})