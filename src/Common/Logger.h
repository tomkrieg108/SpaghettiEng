#pragma once
#include <spdlog/spdlog.h>
#include <type_traits>

namespace Utils
{
	using SpdLogger = std::shared_ptr<spdlog::logger>;

	class Logger
	{
	public:
		static void Initialise();
		static auto& GetDefault() { return s_default_logger; }
		static SpdLogger Create(const std::string& name);
	private:
		static SpdLogger s_default_logger;
		static bool s_initialised;
	};
}

#define SPG_LOGGING_ENABLED_RELEASE

#if defined SPG_DEBUG || defined(SPG_LOGGING_ENABLED_RELEASE)
	#define LOG_DEFAULT_TRACE(...)  Utils::Logger::Initialise();  Utils::Logger::GetDefault()->trace(__VA_ARGS__)
	#define LOG_DEFAULT_INFO(...)   Utils::Logger::Initialise();  Utils::Logger::GetDefault()->info(__VA_ARGS__)
	#define LOG_DEFAULT_WARN(...)   Utils::Logger::Initialise();  Utils::Logger::GetDefault()->warn(__VA_ARGS__)
	#define LOG_DEFAULT_ERROR(...)  Utils::Logger::Initialise();  Utils::Logger::GetDefault()->error(__VA_ARGS__)
	#define LOG_DEFAULT_CRITICAL(...) Utils::Logger::Initialise(); Utils::Logger::GetDefault()->critical(__VA_ARGS__)

	#define LOG_TRACE(logger, ...) \
		static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->trace(__VA_ARGS__);

	#define LOG_INFO(logger, ...) \
		static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->info(__VA_ARGS__);

	#define LOG_WARN(logger, ...) \
		static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->warn(__VA_ARGS__);

	#define LOG_ERROR(logger, ...) \
		static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->error(__VA_ARGS__);

	#define LOG_CRITICAL(logger, ...) \
		static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->info(__VA_ARGS__);				

#else
	#define LOG_DEFAULT_TRACE(...)    
	#define LOG_DEFAULT_INFO(...)     
	#define LOG_DEFAULT_WARN(...)     
	#define LOG_DEFAULT_ERROR(...)    
	#define LOG_DEFAULT_CRITICAL(...) 	

	#define LOG_TRACE(logger, ...)    
	#define LOG_INFO(logger, ...)     
	#define LOG_WARN(logger, ...)     
	#define LOG_ERROR(logger, ...)    
	#define LOG_CRITICAL(logger, ...) 	
#endif	

