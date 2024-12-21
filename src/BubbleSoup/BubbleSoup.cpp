#include <iostream>

//external libs
//should be available because they were linked as public in Geom lib and proparated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Geom/Geom.h"
#include "SpaghettiEng/SpaghettiEng.h"
#include "BubbleSoup.h"

namespace Bbl
{
  void EditorPrintHello()
  {
    std::cout << "Hello From App\n";
    
    #ifdef SPG_DEBUG
      std::cout << "APP: SPG_DEBUG\n";
    #elif defined(SPG_RELEASE)
        std::cout << "APP: SPG_DEBUG\n";
    #endif  

    #ifdef _WIN32
      std::cout << "APP: WIN\n";
    #elif defined(__linux__)
      std::cout << "APP: LINUX\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "APP: x86_64\n";
    #endif

    std::cout << "External libs linked into App exe:\n";
    std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << "\n";
    std::cout << "FMT:" << FMT_VERSION << "\n";
    std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << "\n";
    std::cout << "SPDLOG....:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";
    std::cout << "\n";
    std::cout << "####################################################\n\n";
  }
}

int main()
{
  Geom::GeomLibHello();
  Spg::EngLibHello();
  Bbl::EditorPrintHello();

  Spg::Application app;
  app.Initialise();
  app.Run();
  app.Shutdown();
  return 0;
}