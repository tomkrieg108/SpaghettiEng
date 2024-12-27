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

#define SPG_ENABLE_LOGGING_RELEASE

#if defined SPG_DEBUG || defined(SPG_ENABLE_LOGGING_RELEASE)
	#define SPG_TRACE(...)    ::Spg::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define SPG_INFO(...)     ::Spg::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define SPG_WARN(...)     ::Spg::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define SPG_ERROR(...)    ::Spg::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define SPG_CRITICAL(...) ::Spg::Log::GetCoreLogger()->critical(__VA_ARGS__)

	#define SPG_APP_TRACE(...)    ::Spg::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define SPG_APP_INFO(...)     ::Spg::Log::GetClientLogger()->info(__VA_ARGS__)
	#define SPG_APP_WARN(...)     ::Spg::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define SPG_APP_ERROR(...)    ::Spg::Log::GetClientLogger()->error(__VA_ARGS__)
	#define SPG_APP_CRITICAL(...) ::Spg::Log::GetClientLogger()->critical(__VA_ARGS__)
#else
	#define SPG_TRACE(...)    
	#define SPG_INFO(...)     
	#define SPG_WARN(...)     
	#define SPG_ERROR(...)    
	#define SPG_CRITICAL(...) 

	#define SPG_APP_TRACE(...)    
	#define SPG_APP_INFO(...)     
	#define SPG_APP_WARN(...)
	#define SPG_APP_ERROR(...)
	#define SPG_APP_CRITICAL(...)
#endif
