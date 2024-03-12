#include "fepch.h"
#include "application.h"

#include <glad/glad.h>
#include "input/input.h"

namespace fusion {

	static GLenum ShaderDataTypeToGL(ShaderDataType type)
	{
		switch (type)
		{
			case fusion::ShaderDataType::Float:		return GL_FLOAT;
			case fusion::ShaderDataType::Float2:	return GL_FLOAT;
			case fusion::ShaderDataType::Float3:	return GL_FLOAT;
			case fusion::ShaderDataType::Float4:	return GL_FLOAT;
			case fusion::ShaderDataType::Mat3:		return GL_FLOAT;
			case fusion::ShaderDataType::Mat4:		return GL_FLOAT;
			case fusion::ShaderDataType::Int:		return GL_INT;
			case fusion::ShaderDataType::Int2:		return GL_INT;
			case fusion::ShaderDataType::Int3:		return GL_INT;
			case fusion::ShaderDataType::Int4:		return GL_INT; 
			case fusion::ShaderDataType::Bool:		return GL_BOOL;
		}
		FE_CORE_ASSERT(false, "Unknown shader type");
		return 0;
	}

	Application::Application()
	{
		FE_CORE_ASSERT(!_instance, "Application already exists!");
		_instance = this;

		_window = std::unique_ptr<Window>(Window::create());
		_window->set_event_callback(FE_BIND_EVENT_FN(Application::on_event));

		_imgui_layer = new ImGUILayer();
		push_overlay(_imgui_layer);

		
		glCreateVertexArrays(1, &_vertexArray);

		float vertices[3 * 6] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		};
		_vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		{
			BufferLayout layout = {
				{ ShaderDataType::Float3, "inPosition"},
				{ ShaderDataType::Float3, "inColor"}
			};

			_vertexBuffer->set_layout(layout);
		}

		uint32_t index = 0;
		const auto& layout = _vertexBuffer->get_layout();
		for (const auto& element : layout)
		{
			glVertexArrayAttribFormat(_vertexArray, index, element.GetComponentCount(), ShaderDataTypeToGL(element.type), element.normalized, element.offset);
			glVertexArrayAttribBinding(_vertexArray, index, 0);
			glEnableVertexArrayAttrib(_vertexArray, index);

			index++;
		}
		
		_vertexBuffer->bind(_vertexArray);
		
		//glVertexArrayAttribFormat(_vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
		//glVertexArrayAttribBinding(_vertexArray, 0, 0);
		//glEnableVertexArrayAttrib(_vertexArray, 0);
		
		uint32_t indices[3] = { 0, 1, 2 };
		_indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		
		_indexBuffer->bind(_vertexArray);
		
		glBindVertexArray(0);

		const std::string vertexShader = R"(
			#version 460 core
			layout(location = 0) in vec3 inPosition;
			layout(location = 1) in vec3 inColor;
			
			out vec3 v_position;
			out vec3 v_color;

			void main() {
				v_position = inPosition;
				v_color = inColor;
				gl_Position = vec4(inPosition, 1.0);
			}
		)";

		const std::string fragmentShader = R"(
			#version 460 core
			layout(location = 0) out vec4 color;

			in vec3 v_position;
			in vec3 v_color;

			void main() {
				color = vec4(v_color, 1.0);
			}
		)";

		_shader.reset(Shader::create("base", vertexShader, fragmentShader));
	}

	Application::~Application()
	{
		_shader->unbind();
		_vertexBuffer->unbind(_vertexArray);
		_indexBuffer->unbind(_vertexArray);
		glDisableVertexArrayAttrib(_vertexArray, 0);
		glDeleteVertexArrays(1, &_vertexArray);
	}

	void Application::push_layer(Layer* layer)
	{
		_layer_stack.push_layer(layer);
		layer->on_attach();
	}

	void Application::push_overlay(Layer* layer)
	{
		_layer_stack.push_overlay(layer);
		layer->on_attach();
	}

	void Application::run()
	{		
		while (_running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			_shader->bind();
			glBindVertexArray(_vertexArray);
			glDrawElements(GL_TRIANGLES, _indexBuffer->get_count(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : _layer_stack)
			{
				layer->on_update();
			}

			_imgui_layer->begin();
			for (Layer* layer : _layer_stack)
			{
				layer->on_imgui_render();
			}
			_imgui_layer->end();

			_window->on_update();
		}
	}

	void Application::on_event(Event& e)
	{
		//FE_CORE_INFO("{0}", e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(FE_BIND_EVENT_FN(on_window_closed));

		for (auto it = _layer_stack.end(); it != _layer_stack.begin();)
		{
			if (e.handled)
				break;
			(*--it)->on_event(e);
		}
	}

	bool Application::on_window_closed(WindowCloseEvent& e)
	{
		_running = false;
		return true;
	}


}