#pragma once

namespace fusion {

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

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
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

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
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

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: _elements(elements)
		{
			calculate_offset_and_stride();
		}

		inline const std::vector<BufferElement>& get_elements() const { return _elements; }
		inline uint32_t get_stride() const { return _stride; }

		std::vector<BufferElement>::iterator begin() { return _elements.begin(); }
		std::vector<BufferElement>::iterator end() { return _elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return _elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return _elements.end(); }

	private:
		void calculate_offset_and_stride()
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

		virtual void bind(uint32_t vao) const = 0;
		virtual void unbind(uint32_t vao) const = 0;
		virtual uint32_t get_buffer() const = 0;

		virtual void set_layout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& get_layout() const = 0;

		static VertexBuffer* create(float* vertices, uint32_t size);
	};

	class IndexBuffer 
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void bind(uint32_t vao) const = 0;
		virtual void unbind(uint32_t vao) const = 0;
		virtual uint32_t get_count() const = 0;

		virtual uint32_t get_buffer() const = 0;

		static IndexBuffer* create(uint32_t* indices, uint32_t size);
	};
}