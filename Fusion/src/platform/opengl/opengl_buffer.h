#pragma once

#include "fusion/renderer/buffer.h"

namespace Fusion {

	class OpenGLVertexBuffer : public VertexBuffer 
	{
	public:
		OpenGLVertexBuffer(float* p_vertices, uint32_t p_size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind(uint32_t p_vertexArray) const override;
		virtual void Unbind(uint32_t p_vertexArray) const override;

		virtual inline uint32_t GetBuffer() const override { return _rendererID; }

		virtual inline void SetLayout(const BufferLayout& p_layout) override { _layout = p_layout; }
		virtual inline const BufferLayout& GetLayout() const override { return _layout; }

	private:
		uint32_t _rendererID;
		BufferLayout _layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* p_indices, uint32_t p_size);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind(uint32_t p_vertexArray) const override;
		virtual void Unbind(uint32_t p_vertexArray) const override;
		virtual inline uint32_t GetCount() const override { return _count; }

		virtual inline uint32_t GetBuffer() const override { return _rendererID; }
	private:
		uint32_t _rendererID;
		uint32_t _count;
	};
}