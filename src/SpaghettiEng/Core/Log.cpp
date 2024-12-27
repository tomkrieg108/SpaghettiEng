#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
//#include <spdlog/fmt/ostr.h> // allows custom << operators to be used

namespace Spg
{
  std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
  std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

  void Log::Initialise()
  {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_CoreLogger = spdlog::stdout_color_mt("ENG");
    s_ClientLogger = spdlog::stdout_color_mt("APP");

    s_CoreLogger->set_level(spdlog::level::trace);
    s_ClientLogger->set_level(spdlog::level::trace);
  }
}

