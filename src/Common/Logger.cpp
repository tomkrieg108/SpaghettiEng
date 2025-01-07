#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
//#include <spdlog/fmt/ostr.h> // allows custom << operators to be used

namespace Utils
{
  SpdLogger Logger::s_default_logger;
  bool Logger::s_initialised = false;

  void Logger::Initialise()
  {
    if(s_initialised)
      return;
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_default_logger = spdlog::stdout_color_mt("DEFAULT LOG");
    s_default_logger->set_level(spdlog::level::trace);
    s_initialised = true;
  }
    
  SpdLogger Logger::Create(const std::string& name)
  {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    SpdLogger logger;
    logger = spdlog::stdout_color_mt(name);
    logger->set_level(spdlog::level::trace);
    return logger;
  }
}

