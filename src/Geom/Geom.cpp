#include <iostream>

//external libs
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "Geom.h"

namespace Geom
{
  void GeomLibHello()
  {
    std::cout << "Hello from Geom Lib!\n";
    #ifdef SPG_DEBUG
      std::cout << "GEOM: SPG_DEBUG\n";
    #elif defined(SPG_RELEASE)
        std::cout << "GEOM: SPG_DEBUG\n";
    #endif  

    #ifdef _WIN32
      std::cout << "GEOM: WIN\n";
    #elif defined(__linux__)
      std::cout << "GEOM: LINUX\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "GEOM: x86_64\n";
    #endif
    std::cout << "External libs linked into Geom Lib:\n";
    std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << "\n";
    std::cout << "FMT:" << FMT_VERSION << "\n";
    std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << "\n";
    std::cout << "SPDLOG:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";

    std::cout << "####################################################\n\n";
  }
}