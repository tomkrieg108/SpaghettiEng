#pragma once
#include <spdlog/spdlog.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
//#include <fmt/format.h>

#define SPG_LOGGING_ENABLED_RELEASE	

namespace Core
{
	using SpdLogger = std::shared_ptr<spdlog::logger>;

	class Logger
	{
	public:
     // Initializes the core application logger and shared sinks
     // Currently called in void Application::SystemInit() Application.cppp (EngLib)
		static void Initialise();

    // Only for the main executable/core messages
		static auto& GetDefault() { return s_default_logger; }

    // Dynamically creates a new domain logger sharing the same destination sinks
		static SpdLogger Create(const std::string& name);

	private:
		static SpdLogger s_default_logger;
    static std::vector<spdlog::sink_ptr> s_shared_sinks; // Keeps destinations unified
		static bool s_initialised;
	};
}

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L,T,Q>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::vec<L,T,Q>& v, FormatContext& ctx) const  {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(v));
	}
};

template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C,R,T,Q>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::mat<C,R,T,Q>& m, FormatContext& ctx) const  {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(m));
	}
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::qua<T, Q>& q, FormatContext& ctx) const  {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(q));
	}
};


#if defined SPG_DEBUG || defined(SPG_LOGGING_ENABLED_RELEASE)
	#define SPG_TRACE(...) Core::Logger::GetDefault()->trace(__VA_ARGS__);
	#define SPG_INFO(...)  Core::Logger::GetDefault()->info(__VA_ARGS__);
	#define SPG_WARN(...)  Core::Logger::GetDefault()->warn(__VA_ARGS__);
	#define SPG_ERROR(...) Core::Logger::GetDefault()->error(__VA_ARGS__);
	#define SPG_CRITICAL(...) Core::Logger::GetDefault()->critical(__VA_ARGS__);
	#define SPG_LOG_FLUSH Core::Logger::GetDefault()->flush();

	
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

