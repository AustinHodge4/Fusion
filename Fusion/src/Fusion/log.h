#pragma once

#include <memory>

#include "core.h"
#include "spdlog/spdlog.h"

namespace fusion {
	class FUSION_API log
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& get_core_logger() { return _core_logger; }
		inline static std::shared_ptr<spdlog::logger>& get_client_logger() { return _client_logger; }
	private:
		static std::shared_ptr<spdlog::logger> _core_logger;
		static std::shared_ptr<spdlog::logger> _client_logger;
	};
}

// core log macros
#define FE_CORE_TRACE(...) ::fusion::log::get_core_logger()->trace(__VA_ARGS__)
#define FE_CORE_INFO(...) ::fusion::log::get_core_logger()->info(__VA_ARGS__)
#define FE_CORE_WARN(...) ::fusion::log::get_core_logger()->warn(__VA_ARGS__)
#define FE_CORE_ERROR(...) ::fusion::log::get_core_logger()->error(__VA_ARGS__)
#define FE_CORE_FATAL(...) ::fusion::log::get_core_logger()->fatal(__VA_ARGS__)

// client log macros
#define FE_TRACE(...) ::fusion::log::get_client_logger()->trace(__VA_ARGS__)
#define FE_INFO(...) ::fusion::log::get_client_logger()->info(__VA_ARGS__)
#define FE_WARN(...) ::fusion::log::get_client_logger()->warn(__VA_ARGS__)
#define FE_ERROR(...) ::fusion::log::get_client_logger()->error(__VA_ARGS__)
#define FE_FATAL(...) ::fusion::log::get_client_logger()->fatal(__VA_ARGS__)