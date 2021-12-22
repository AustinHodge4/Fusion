#pragma once

#include <engine/vk_types.hpp>
#include <shaderc/shaderc.hpp>

namespace vk_shader {
	bool is_spir_v(std::string_view p_fileName);
	shaderc_shader_kind get_file_kind(std::string p_fileName);
}