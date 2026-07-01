#include "SpgApp/SpgApp.h"

#include <string>
using namespace std::string_literals;

namespace Spg
{
  Application* CreateApplication()
  {
    return new SpgApp("Spg App"s);
  }

  SpgApp::SpgApp(const std::string& title) :
    Spg::Application(title)
  {}

  SpgApp::~SpgApp()
  {
  }
}

int main()
{
  Spg::Application::SystemInit();
  auto app = Spg::CreateApplication();
  app->Run();
}