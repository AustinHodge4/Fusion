#include <engine/vk_shader.hpp>

bool vk_shader::is_spir_v(std::string_view p_fileName)
{
	const std::string_view extention = p_fileName.substr(p_fileName.size() - 4, 4);
	return extention == ".spv";
}

shaderc_shader_kind vk_shader::get_file_kind(std::string p_fileName)
{
    // Get file extension
    const auto dotIndex = p_fileName.find_last_of('.');
    const auto extension = p_fileName.substr(dotIndex + 1);

    // Fall-back in case we cannot infer type from extension
    shaderc_shader_kind kind = shaderc_shader_kind::shaderc_glsl_infer_from_source;

    if (extension == "comp")
        kind = shaderc_shader_kind::shaderc_glsl_default_compute_shader;
    else if (extension == "vert")
        kind = shaderc_shader_kind::shaderc_glsl_default_vertex_shader;
    else if (extension == "frag")
        kind = shaderc_shader_kind::shaderc_glsl_default_fragment_shader;
    else if (extension == "geom")
        kind = shaderc_shader_kind::shaderc_glsl_default_geometry_shader;

    return kind;
}
