#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace fusion {
	std::shared_ptr<spdlog::logger> log::_core_logger;
	std::shared_ptr<spdlog::logger> log::_client_logger;

	void log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		_core_logger = spdlog::stdout_color_mt("Fusion");
		_core_logger->set_level(spdlog::level::trace);

		_client_logger = spdlog::stdout_color_mt("App");
		_client_logger->set_level(spdlog::level::trace);
	}
}