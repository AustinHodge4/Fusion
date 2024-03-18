#pragma once

namespace Fusion {

	enum class ShaderDataType : uint8_t
	{
		None = 0,
		Float,
		Float2, 
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType p_type)
	{
		switch (p_type)
		{
			case ShaderDataType::Float:    return 4;
			case ShaderDataType::Float2:   return 4 * 2;
			case ShaderDataType::Float3:   return 4 * 3;
			case ShaderDataType::Float4:   return 4 * 4;
			case ShaderDataType::Mat3:     return 4 * 3 * 3;
			case ShaderDataType::Mat4:     return 4 * 4 * 4;
			case ShaderDataType::Int:      return 4;
			case ShaderDataType::Int2:     return 4 * 2;
			case ShaderDataType::Int3:     return 4 * 3;
			case ShaderDataType::Int4:     return 4 * 4;
			case ShaderDataType::Bool:     return 1;
		}

		FE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t size;
		size_t offset;
		bool normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType p_type, const std::string& p_name, bool p_normalized = false)
			: name(p_name), type(p_type), size(ShaderDataTypeSize(p_type)), offset(0), normalized(p_normalized)
		{}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
				case ShaderDataType::Float:   return 1;
				case ShaderDataType::Float2:  return 2;
				case ShaderDataType::Float3:  return 3;
				case ShaderDataType::Float4:  return 4;
				case ShaderDataType::Mat3:    return 3; 
				case ShaderDataType::Mat4:    return 4;
				case ShaderDataType::Int:     return 1;
				case ShaderDataType::Int2:    return 2;
				case ShaderDataType::Int3:    return 3;
				case ShaderDataType::Int4:    return 4;
				case ShaderDataType::Bool:    return 1;
			}

			FE_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;

		BufferLayout(const std::initializer_list<BufferElement>& p_elements)
			: _elements(p_elements)
		{
			CalculateStrideAndOffset();
		}

		inline const std::vector<BufferElement>& GetElements() const { return _elements; }
		inline uint32_t GetStride() const { return _stride; }

		std::vector<BufferElement>::iterator begin() { return _elements.begin(); }
		std::vector<BufferElement>::iterator end() { return _elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return _elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return _elements.end(); }

	private:
		void CalculateStrideAndOffset()
		{
			uint32_t offset = 0;
			_stride = 0;
			for (auto& element : _elements)
			{
				element.offset = offset;
				offset += element.size;
				_stride += element.size;
			}
		}
	private:
		std::vector<BufferElement> _elements;
		uint32_t _stride = 0;
	};

	class VertexBuffer 
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind(uint32_t p_vertexArray) const = 0;
		virtual void Unbind(uint32_t p_vertexArray) const = 0;
		virtual uint32_t GetBuffer() const = 0;

		virtual void SetLayout(const BufferLayout& p_layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static VertexBuffer* Create(float* p_vertices, uint32_t p_size);
	};

	class IndexBuffer 
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind(uint32_t p_vertexArray) const = 0;
		virtual void Unbind(uint32_t p_vertexArray) const = 0;
		virtual uint32_t GetCount() const = 0;

		virtual uint32_t GetBuffer() const = 0;

		static IndexBuffer* Create(uint32_t* p_indices, uint32_t p_size);
	};
}