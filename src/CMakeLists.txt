add_subdirectory(Geom)
add_subdirectory(SpaghettiEng)
add_subdirectory(BubbleSoup)

target_include_directories(${LIB_GEOM}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${LIB_ENG}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${APP_EDITOR}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 