#pragma once
#include <spdlog/spdlog.h>

namespace Spg
{
	class Log
	{
	public:
		static void Initialise();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define LOG_TRACE(...)    ::Spg::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Spg::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Spg::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Spg::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Spg::Log::GetCoreLogger()->critical(__VA_ARGS__)

// #define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
// #define LOG_INFO(...) spdlog::info(__VA_ARGS__)
// #define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
// #define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
// #define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

