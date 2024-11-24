#include <iostream>

//external libs
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "Geom.h"

void GeomLibHello()
{
#ifdef _WIN32
  std::cout << "WIN GEOM\n";
 #elif defined(__linux__)
  std::cout << "LINUX GEOM\n";
#endif

#if defined(__x86_64__) || defined(_M_X64)
  std::cout << "x86_64 GEOM\n";
#endif



  std::cout << "\nHello from Geom Lib!\n";
  std::cout << "External libs linked into Geom Lib:\n";

  std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
            << NLOHMANN_JSON_VERSION_PATCH << "\n";

  std::cout << "FMT:" << FMT_VERSION << "\n";

  std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
            << "\n";

  std::cout << "SPDLOG:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";


}