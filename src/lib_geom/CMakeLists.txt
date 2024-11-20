set(GEOM_SOURCES "geom.cpp")
set(GEOM_HEADERS "geom.h")
set(GEOM_INCLUDES "./")

add_library(${LIB_GEOM} STATIC ${GEOM_SOURCES} ${GEOM_HEADERS} )
target_include_directories(${LIB_GEOM} PUBLIC ${GEOM_INCLUDES})
target_link_libraries(${LIB_GEOM} PUBLIC 
  #project_name::bibrary name (usually the same but, may have header only option => different lib name)
  #look in CMakeLists.txt for the project to see
  nlohmann_json::nlohmann_json  
  fmt::fmt
  spdlog::spdlog
  cxxopts::cxxopts
)

# target_link_libraries(${LIB_GEOM} PRIVATE glm::glm) # can apparently do this too according to Github
target_link_libraries(${LIB_GEOM} PRIVATE glm::glm-header-only) 