#pragma once

#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace fusion {
	class FUSION_API Log
	{
	public:
		static void init();

		inline static const std::shared_ptr<spdlog::logger>& get_core_logger() { return _core_logger; }
		inline static const std::shared_ptr<spdlog::logger>& get_client_logger() { return _client_logger; }
	private:
		static std::shared_ptr<spdlog::logger> _core_logger;
		static std::shared_ptr<spdlog::logger> _client_logger;
	};
}

// core log macros
#define FE_CORE_TRACE(...) ::fusion::Log::get_core_logger()->trace(__VA_ARGS__)
#define FE_CORE_INFO(...) ::fusion::Log::get_core_logger()->info(__VA_ARGS__)
#define FE_CORE_WARN(...) ::fusion::Log::get_core_logger()->warn(__VA_ARGS__)
#define FE_CORE_ERROR(...) ::fusion::Log::get_core_logger()->error(__VA_ARGS__)
#define FE_CORE_FATAL(...) ::fusion::Log::get_core_logger()->critical(__VA_ARGS__)

// client log macros
#define FE_TRACE(...) ::fusion::Log::get_client_logger()->trace(__VA_ARGS__)
#define FE_INFO(...) ::fusion::Log::get_client_logger()->info(__VA_ARGS__)
#define FE_WARN(...) ::fusion::Log::get_client_logger()->warn(__VA_ARGS__)
#define FE_ERROR(...) ::fusion::Log::get_client_logger()->error(__VA_ARGS__)
#define FE_FATAL(...) ::fusion::Log::get_client_logger()->critical(__VA_ARGS__)