#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/sinks/basic_file_sink.h> // Uncomment if you add file logging later
//#include <spdlog/fmt/ostr.h> // allows custom << operators to be used

namespace Core
{
  SpdLogger Logger::s_default_logger;
  std::vector<spdlog::sink_ptr> Logger::s_shared_sinks;
  bool Logger::s_initialised = false;

  void Logger::Initialise()
  {
    if(s_initialised)
      return;

    // 1. Define your destinations (sinks) once
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("%^[%T] [%n] %v%$"); // [%n] will show the library name  

    s_shared_sinks.push_back(console_sink);
    // s_shared_sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("app.log")); // Easy expansion   

    // 2. Create the core app logger using these sinks
    s_default_logger = std::make_shared<spdlog::logger>("CORE", s_shared_sinks.begin(), s_shared_sinks.end());
    s_default_logger->set_level(spdlog::level::trace);

    // Register it globally with spdlog
    spdlog::register_logger(s_default_logger);

    s_initialised = true;  
  }
    
  SpdLogger Logger::Create(const std::string& name)
  {
    // Force initialization if forgot
    if (!s_initialised) 
      Initialise();

    // Prevent duplicates and crashes
    if (auto existing = spdlog::get(name)) {
        return existing;
    }

    // Create a totally independent logger instance, but routing to the SAME sinks
    auto logger = std::make_shared<spdlog::logger>(name, s_shared_sinks.begin(), s_shared_sinks.end());
    logger->set_level(spdlog::level::trace); // Can be customized per library later
    
    spdlog::register_logger(logger);
    return logger;
  }
}

