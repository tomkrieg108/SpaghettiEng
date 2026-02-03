#include "Geometry.h"
#include <Common/Common.h>
//external libs check only
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
//------------------------
#include <glm/glm.hpp>
#include <iostream>


namespace Geom
{
  void HelloGeomLib()
  {
    SPG_WARN("Hello from Geom Lib!");

    #ifdef SPG_DEBUG
      SPG_TRACE("APP: SPG_DEBUG defined");
    #elif defined(SPG_RELEASE)
        SPG_TRACE("APP: SPG_RELEASE defined");
    #endif  

    #ifdef _WIN32
      SPG_TRACE("APP: WIN platform");
    #elif defined(__linux__)
      SPG_TRACE("APP: LINUX platform");
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      SPG_TRACE("APP: x86_64 architecture");
    #endif

    SPG_WARN("External libs linked into UTILS Lib");
    SPG_TRACE("JSON: {}.{}.{}", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH );
    SPG_TRACE( "FMT: {} ", FMT_VERSION);
    SPG_TRACE("CXXOPTS: {}.{}.{}",CXXOPTS__VERSION_MAJOR,CXXOPTS__VERSION_MAJOR, CXXOPTS__VERSION_PATCH);
    SPG_TRACE("SPDLOG: {},{}.{}",SPDLOG_VER_MAJOR,SPDLOG_VER_MINOR,SPDLOG_VER_PATCH);
  
    auto logger = Utils::Logger::Create("GEOM");
    LOG_TRACE(logger, "SPDLOG: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    
    glm::vec3 v{42,42,42};
    std::cout << "vec3 value is: " << v.x << "," << v.y << "," << v.z << "\n";
    std::cout << "\n";
    LOG_TRACE(logger, "That's all the external libs!");
    SPG_WARN( "####################################################");
  }
}