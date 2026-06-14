
//Paired header
#include "Core/Init.h"

//C System headers

//Std lib headers
#include <iostream>
#include <filesystem>

//3rd party external libs
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

//Project headers
#include "Core/PlatformDetect/PlatformDetect.h"
#include "Core/Logger.h"


namespace Core
{
  namespace fs = std::filesystem;

  void PrintPlatformAndBuildInfo()
  {
    SPG_INFO("Platform and Build:");
    SPG_TRACE("Architecture: {} (via ArchDetech.h) ", ARCH_NAME)
    SPG_TRACE("Compiler: {} (via CompilerDetech.h) ", COMPILER_NAME)
    SPG_TRACE("OS: {} (via OSDetech.h) ", OS_NAME)
    #if defined(SPG_DEBUG)
      SPG_TRACE("SPG_DEBUG defined (via cmake)");
    #elif defined(SPG_RELEASE)
        SPG_TRACE("SPG_RELEASE defined (via cmake)");
    #endif
  }

  void Init()
  {
    
    SPG_WARN("Core Lib Init:");

    #if defined(SPG_DEBUG)
      SPG_TRACE("SPG_DEBUG defined (via cmake)");
    #elif defined(SPG_RELEASE)
        SPG_TRACE("SPG_RELEASE defined (via cmake)");
    #endif

    #if defined(SPG_WIN64)
      SPG_TRACE("SPG_WIN64 defined (via cmake)");
    #elif defined(SPG_LINUX)
        SPG_TRACE("SPG_LINUX defined (via cmake)");
    #endif

    SPG_TRACE("Architecture: {} (via ArchDetech.h) ", ARCH_NAME)
    SPG_TRACE("Compiler: {} (via CompilerDetech.h) ", COMPILER_NAME)
    SPG_TRACE("OS: {} (via OSDetech.h) ", OS_NAME)
    
    #define VALUE 99
    std::cout << STRINGIFY(VALUE);
    std::cout << STRINGIFY_IMPL(VALUE);
    
  
    //auto logger = Core::Logger::Create("UTILS");

    SPG_WARN("External libs linked into UTILS Lib");
    SPG_TRACE("JSON: {}.{}.{}", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH );
    SPG_TRACE( "FMT: {} ", FMT_VERSION);
    SPG_TRACE("CXXOPTS: {}.{}.{}",CXXOPTS__VERSION_MAJOR,CXXOPTS__VERSION_MAJOR, CXXOPTS__VERSION_PATCH);
    SPG_TRACE("SPDLOG: {},{}.{}",SPDLOG_VER_MAJOR,SPDLOG_VER_MINOR,SPDLOG_VER_PATCH);
    
    
    SPG_TRACE("SPDLOG: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    SPG_TRACE("That's all the external libs!");

    fs::path current_path = fs::current_path();
    fs::path source_file_path(__FILE__);
    SPG_TRACE("Current working directory is: {}", current_path.string());
    SPG_TRACE("This file is: {}", source_file_path.string());

    SPG_WARN( "####################################################");
    
  }
}