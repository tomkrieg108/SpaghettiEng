add_subdirectory(CoreLib)
add_subdirectory(MathLib)
add_subdirectory(Geometry)
add_subdirectory(PhysLib)
add_subdirectory(SpaghettiEng)
add_subdirectory(GeomApp)
add_subdirectory(SpgApp)

target_include_directories(${LIB_CORE}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${LIB_MATH}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${LIB_GEOM}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${LIB_CYCLONE}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${LIB_ENG}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${APP_GEOM}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

target_include_directories(${APP_SPG}  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
) 

if(ENABLE_TESTING)
  add_subdirectory(Tests)

  #this causes an error?
  # target_include_directories(${TEST_GEOM}  PUBLIC 
  # "${CMAKE_CURRENT_SOURCE_DIR}"
endif()

