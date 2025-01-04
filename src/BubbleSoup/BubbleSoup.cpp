#include <iostream>

//external libs
//should be available because they were linked as public in Geom lib and proparated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Geom/Geom.h"
#include "BubbleSoup.h"

using namespace std::string_literals;

namespace Spg
{
  DefaultLayer::DefaultLayer(const std::string& name) : Layer(name)  {}

  void DefaultLayer::OnImGuiRender()
  {
    ImGui::Begin(m_name.c_str());
    if (ImGui::CollapsingHeader("Gui For this layer"))
    {
      ImGui::Text("Data and controls for this layer goes here");
    }
    ImGui::End();
  }

  BubbleSoup::BubbleSoup(const std::string& title) : Application(title) {}

  void AppPrintHello()
  {
    std::cout << "Hello From App\n";
    
    #ifdef SPG_DEBUG
      std::cout << "APP: SPG_DEBUG defined\n";
    #elif defined(SPG_RELEASE)
        std::cout << "APP: SPG_RELEASE defined\n";
    #endif  

    #ifdef _WIN32
      std::cout << "APP: WIN platform\n";
    #elif defined(__linux__)
      std::cout << "APP: LINUX platform\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "APP: x86_64 architecture\n";
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
  Spg::AppPrintHello();

  Spg::BubbleSoup bubble_soup("Bubble Soup"s);
  Spg::DefaultLayer* default_layer = new Spg::DefaultLayer(std::string("Default Layer"));
  bubble_soup.PushLayer(default_layer);
  bubble_soup.Initialise();
  bubble_soup.Run();
  bubble_soup.PopLayer(default_layer);
  bubble_soup.Shutdown();
  return 0;
}