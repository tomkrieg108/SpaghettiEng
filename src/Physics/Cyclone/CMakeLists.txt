add_library(${LIB_CYCLONE} STATIC 
  "./Core.h"
  "./Core.cpp"
  "./Particle.h"
  "./Particle.cpp"
  "./ParticleForceGen.h"
  "./ParticleForceGen.cpp"
  "./ParticleContacts.h"
  "./ParticleContacts.cpp"
  "./ParticleLinks.h"
  "./ParticleLinks.cpp"
  "./ParticleWorld.h"
  "./ParticleWorld.cpp"
  "./BallisticsDemo.h"
  "./BallisticsDemo.cpp"
  "./FireworksDemo.h"
  "./FireworksDemo.cpp"
)

target_include_directories(${LIB_CYCLONE} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_CYCLONE} PUBLIC ${LIB_COMMON} )
target_link_libraries(${LIB_CYCLONE} PUBLIC glm::glm-header-only) 
target_link_libraries(${LIB_CYCLONE} PUBLIC ${LIB_MATH} )