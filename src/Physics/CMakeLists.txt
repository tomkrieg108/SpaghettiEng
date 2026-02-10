add_library(${LIB_PHYS} STATIC 
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
  "./BallisticsDemo.h"
  "./BallisticsDemo.cpp"
  "./FireworksDemo.h"
  "./FireworksDemo.cpp"
)

target_include_directories(${LIB_PHYS} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_PHYS} PUBLIC ${LIB_COMMON} )
target_link_libraries(${LIB_PHYS} PUBLIC glm::glm-header-only) 
target_link_libraries(${LIB_PHYS} PUBLIC ${LIB_MATH} )