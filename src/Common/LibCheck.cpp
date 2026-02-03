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

  void HelloUtilsLib()
  {
    
    SPG_WARN("Hello from Common Utils Lib!");

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

    //Utils::Logger::Initialise();
    auto logger = Utils::Logger::Create("UTILS");

    SPG_WARN("External libs linked into UTILS Lib");
    SPG_TRACE("JSON: {}.{}.{}", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH );
    SPG_TRACE( "FMT: {} ", FMT_VERSION);
    SPG_TRACE("CXXOPTS: {}.{}.{}",CXXOPTS__VERSION_MAJOR,CXXOPTS__VERSION_MAJOR, CXXOPTS__VERSION_PATCH);
    SPG_TRACE("SPDLOG: {},{}.{}",SPDLOG_VER_MAJOR,SPDLOG_VER_MINOR,SPDLOG_VER_PATCH);
    
    
    LOG_TRACE(logger, "SPDLOG: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    LOG_TRACE(logger, "That's all the external libs!");

    fs::path current_path = fs::current_path();
    fs::path source_file_path(__FILE__);
    LOG_TRACE(logger, "Current working directory is: {}", current_path.string());
    LOG_TRACE(logger, "This file is: {}", source_file_path.string());

    SPG_WARN( "####################################################");
    
  }
}