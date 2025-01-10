#include <iostream>
#include <filesystem>

//external libs
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Logger.h"
#include "LibCheck.h"

namespace Utils
{
  namespace fs = std::filesystem;

  void LibCheck()
  {
    {
    std::cout << "Hello from Common Utils Lib!\n";
    #ifdef SPG_DEBUG
      std::cout << "UTILS: SPG_DEBUG defined\n";
    #elif defined(SPG_RELEASE)
        std::cout << "UTILS: SPG_RELEASE defined\n";
    #endif  

    #ifdef _WIN32
      std::cout << "UTILS: WIN platform\n";
    #elif defined(__linux__)
      std::cout << "UTILS: LINUX platform\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "UTILS: x86_64 architecture\n";
    #endif

    //Utils::Logger::Initialise();
    auto logger = Utils::Logger::Create("UTILS");
    
    SPG_WARN("External libs linked into UTILS Lib");
    std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << "\n";
    std::cout << "FMT:" << FMT_VERSION << "\n";
    std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << "\n";
    std::cout << "SPDLOG:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";

    LOG_WARN(logger, "SPDLOG: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    LOG_WARN(logger, "That's all the external libs!");

    fs::path current_path = fs::current_path();
    fs::path source_file_path(__FILE__);
    LOG_WARN(logger, "Current working directory is: {}", current_path.string());
    LOG_WARN(logger, "This file is: {}", source_file_path.string());

    std::cout << "####################################################\n\n";
  }
  }
}