#include <iostream>

//external libs
//should be available because they were linked as public in Geom lib and proparated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "SpaghettiEng/SpaghettiEng.h"
#include "BubbleSoup.h"

void EditorPrintHello()
{
  std::cout << "\nHello From Editor\n";

  std::cout << "External libs linked into Editor exe:\n";

  std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
            << NLOHMANN_JSON_VERSION_PATCH << "\n";

  std::cout << "FMT:" << FMT_VERSION << "\n";

  std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
            << "\n";

  std::cout << "SPDLOG....:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";
  std::cout << "\n";

}

int main()
{
  EditorPrintHello();
  EngLibHello();

  spg::Window window;
  window.Init();
  window.RenderLoop();

  return 0;
}