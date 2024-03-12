#pragma once

#include "fusion/renderer/buffer.h"

namespace fusion {

	class OpenGLVertexBuffer : public VertexBuffer 
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void bind(uint32_t vao) const override;
		virtual void unbind(uint32_t vao) const override;

		virtual inline uint32_t get_buffer() const override { return _renderer_id; }

		virtual inline void set_layout(const BufferLayout& layout) override { _layout = layout; }
		virtual inline const BufferLayout& get_layout() const override { return _layout; }

	private:
		uint32_t _renderer_id;
		BufferLayout _layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		virtual void bind(uint32_t vao) const override;
		virtual void unbind(uint32_t vao) const override;
		virtual inline uint32_t get_count() const override { return _count; }

		virtual inline uint32_t get_buffer() const override { return _renderer_id; }
	private:
		uint32_t _renderer_id;
		uint32_t _count;
	};
}