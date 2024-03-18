#pragma once

#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Fusion {
	class FUSION_API Log
	{
	public:
		static void Initialize();

		inline static const std::shared_ptr<spdlog::logger>& GetCoreLogger() { return _coreLogger; }
		inline static const std::shared_ptr<spdlog::logger>& GetClientLogger() { return _clientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> _coreLogger;
		static std::shared_ptr<spdlog::logger> _clientLogger;
	};
}

// core log macros
#define FE_CORE_TRACE(...) ::Fusion::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define FE_CORE_INFO(...) ::Fusion::Log::GetCoreLogger()->info(__VA_ARGS__)
#define FE_CORE_WARN(...) ::Fusion::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define FE_CORE_ERROR(...) ::Fusion::Log::GetCoreLogger()->error(__VA_ARGS__)
#define FE_CORE_FATAL(...) ::Fusion::Log::GetCoreLogger()->critical(__VA_ARGS__)

// client log macros
#define FE_TRACE(...) ::Fusion::Log::GetClientLogger()->trace(__VA_ARGS__)
#define FE_INFO(...) ::Fusion::Log::GetClientLogger()->info(__VA_ARGS__)
#define FE_WARN(...) ::Fusion::Log::GetClientLogger()->warn(__VA_ARGS__)
#define FE_ERROR(...) ::Fusion::Log::GetClientLogger()->error(__VA_ARGS__)
#define FE_FATAL(...) ::Fusion::Log::GetClientLogger()->critical(__VA_ARGS__)