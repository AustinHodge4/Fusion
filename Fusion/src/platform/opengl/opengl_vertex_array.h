#pragma once

#include "fusion/renderer/vertex_array.h"

namespace Fusion {

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& p_vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& p_indexBuffer) override;
		virtual inline const uint32_t GetBuffer() const override { return _rendererID; }

		virtual inline const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return _vertexBuffers; }
		virtual inline const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return _indexBuffer; }
	private:
		uint32_t _rendererID;
		std::vector<std::shared_ptr<VertexBuffer>> _vertexBuffers;
		std::shared_ptr<IndexBuffer> _indexBuffer;
	};
}