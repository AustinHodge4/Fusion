#pragma once

#include "fusion/renderer/texture.h"

namespace Fusion {
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return _width; }
		inline virtual uint32_t GetHeight() const override { return _height; }

		virtual void Bind(uint32_t p_slot = 0) const override;
	private:
		uint32_t _rendererID;

		uint32_t _width;
		uint32_t _height;
	};
}