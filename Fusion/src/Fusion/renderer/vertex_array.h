#pragma once

#include "buffer.h"

#include <memory>

namespace Fusion {
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& p_vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& p_indexBuffer) = 0;
		virtual const uint32_t GetBuffer() const = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		static VertexArray* Create();
	};
}