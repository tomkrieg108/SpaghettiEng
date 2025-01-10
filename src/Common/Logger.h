#pragma once
#include <spdlog/spdlog.h>
#include <type_traits>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define SPG_LOGGING_ENABLED_RELEASE	

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

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}


#if defined SPG_DEBUG || defined(SPG_LOGGING_ENABLED_RELEASE)
	#define SPG_TRACE(...) {Utils::Logger::Initialise();  Utils::Logger::GetDefault()->trace(__VA_ARGS__);}
	#define SPG_INFO(...)  {Utils::Logger::Initialise();  Utils::Logger::GetDefault()->info(__VA_ARGS__);}
	#define SPG_WARN(...)  {Utils::Logger::Initialise();   Utils::Logger::GetDefault()->warn(__VA_ARGS__);}
	#define SPG_ERROR(...)  {Utils::Logger::Initialise();  Utils::Logger::GetDefault()->error(__VA_ARGS__);}
	#define SPG_CRITICAL(...) {Utils::Logger::Initialise(); Utils::Logger::GetDefault()->critical(__VA_ARGS__);}

	#define LOG_TRACE(logger, ...) \
		{static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->trace(__VA_ARGS__);}

	#define LOG_INFO(logger, ...) \
		{static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->info(__VA_ARGS__);}

	#define LOG_WARN(logger, ...) \
		{static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->warn(__VA_ARGS__);}

	#define LOG_ERROR(logger, ...) \
		{static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->error(__VA_ARGS__);}

	#define LOG_CRITICAL(logger, ...) \
		{static_assert(std::is_same_v<decltype(logger), Utils::SpdLogger>); logger->info(__VA_ARGS__);}	

#else
	#define SPG_TRACE(...)    
	#define SPG_INFO(...)     
	#define SPG_WARN(...)     
	#define SPG_ERROR(...)    
	#define SPG_CRITICAL(...) 	

	#define LOG_TRACE(logger, ...)    
	#define LOG_INFO(logger, ...)     
	#define LOG_WARN(logger, ...)     
	#define LOG_ERROR(logger, ...)    
	#define LOG_CRITICAL(logger, ...) 	
#endif	

