#pragma once
#include <spdlog/spdlog.h>
//#include <spdlog/sinks/stdout_color_sinks.h>
//#include <spdlog/fmt/ostr.h> // allows custom << operators to be used

namespace spg
{
	class Log
	{
		//This is a super awesome class - Uses the default logger for now (stdout, multi-threaded, colored)
		//i.e. same as that returned by spdlog::stdout_color_mt("LOGGER NAME");
	public:
		static void Init()
		{
			//spdlog::set_pattern("%^[%T] %n: %v%$");
    	spdlog::set_pattern("%^[%T] %v%$");
		}
	};
}

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

