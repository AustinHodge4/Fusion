#include "fepch.h"
#include "opengl_texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Fusion {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& p_path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* pixels = stbi_load(p_path.c_str(), &width, &height, &channels, 0);

		if (!pixels) {
			FE_CORE_ERROR("[Texture] {0}", stbi_failure_reason());
			FE_CORE_ASSERT(pixels, "Failed to load texture with stb_image");
		}

		_width = width;
		_height = height;

		GLenum internalFormat = 0, pixelsFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			pixelsFormat = GL_RGBA;
		}
		else if (channels == 3) 
		{
			internalFormat = GL_RGB8;
			pixelsFormat = GL_RGB;
		}

		FE_CORE_ASSERT(internalFormat & pixelsFormat, "[Texture] texture format not supported!");
		
		glCreateTextures(GL_TEXTURE_2D, 1, &_rendererID);
		glTextureStorage2D(_rendererID, 1, internalFormat, _width, _height);

		//glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(_rendererID, 0, 0, 0, width, height, pixelsFormat, GL_UNSIGNED_BYTE, pixels);

		stbi_image_free(pixels);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &_rendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t p_slot) const
	{
		glBindTextureUnit(p_slot, _rendererID);
	}
}