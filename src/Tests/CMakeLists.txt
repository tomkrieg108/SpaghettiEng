#include(catch)

add_executable(${TEST_GEOM} 
  "./main.cpp"
)

target_include_directories(${TEST_GEOM} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${TEST_GEOM} PUBLIC ${LIB_GEOM})
target_link_libraries(${TEST_GEOM} PUBLIC Catch2WithMain) #Ref migrate-v2-to-v3 in catch2 docs on Github

#catch_discover_tests(${TEST_GEOM})

