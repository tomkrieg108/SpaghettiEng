#include <iostream>

//external libs check only
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
//------------------------

#include <glm/glm.hpp>

#include "Common/Common.h"
#include "Geometry.h"

namespace Geom
{
  void GeomLibHello()
  {
    std::cout << "Hello from Geom Lib!\n";
    #ifdef SPG_DEBUG
      std::cout << "GEOM: SPG_DEBUG defined\n";
    #elif defined(SPG_RELEASE)
        std::cout << "GEOM: SPG_RELEASE defined\n";
    #endif  

    #ifdef _WIN32
      std::cout << "GEOM: WIN platform\n";
    #elif defined(__linux__)
      std::cout << "GEOM: LINUX platform\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "GEOM: x86_64 architecture\n";
    #endif

    //Utils::Logger::Initialise();
    auto logger = Utils::Logger::Create("GEOM");
    
    SPG_WARN("External libs linked into Geom Lib");
    std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << "\n";
    std::cout << "FMT:" << FMT_VERSION << "\n";
    std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << "\n";
    std::cout << "SPDLOG:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";

    LOG_WARN(logger, "SPDLOG: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);

    glm::vec3 v{42,42,42};
    std::cout << "vec3 value is: " << v.x << "," << v.y << "," << v.z << "\n";
    std::cout << "\n";
    
    LOG_WARN(logger, "That's all the external libs!");

    std::cout << "####################################################\n\n";
  }
}