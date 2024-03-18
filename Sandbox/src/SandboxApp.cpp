#include <fusion.h>

#include <imgui.h>

class ExampleLayer : public fusion::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void on_update() override
	{
		if (fusion::Input::is_key_pressed(fusion::Key::W))
		{
			FE_TRACE("Pressed W key (poll)");
		}
		
	}

	void on_imgui_render() override
	{
		ImGui::Begin("test");
		ImGui::Text("Hello world :)");
		ImGui::End();
	}

	void on_event(fusion::Event& e) override
	{
		if (e.get_event_type() == fusion::EventType::KeyPressed) {
			fusion::KeyPressedEvent& event = (fusion::KeyPressedEvent&)e;
			if (event.get_key() == fusion::Key::W) {
				FE_TRACE("Pressed {0} key (event)!", fusion::Key::get_key_details(fusion::Key::W)->get_display_name());
			}
			FE_TRACE("{0}", event.get_key().get_name());
		}
	}
};

class RenderLayer : public fusion::Layer
{
public:
	RenderLayer() : Layer("Render") 
	{
		_vertex_array.reset(fusion::VertexArray::create());

		float vertices[3 * 6] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		};
		std::shared_ptr<fusion::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(fusion::VertexBuffer::create(vertices, sizeof(vertices)));
		{
			fusion::BufferLayout layout = {
				{ fusion::ShaderDataType::Float3, "inPosition"},
				{ fusion::ShaderDataType::Float3, "inColor"}
			};

			vertexBuffer->set_layout(layout);
		}
		_vertex_array->add_vertex_buffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<fusion::IndexBuffer> indexBuffer;
		indexBuffer.reset(fusion::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		_vertex_array->set_index_buffer(indexBuffer);

		_vertex_array->unbind();

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

		_shader.reset(fusion::Shader::create("base", vertexShader, fragmentShader));
	}

	~RenderLayer() 
	{
		_vertex_array->unbind();
		_shader->unbind();
	}

	void on_update() override
	{
		fusion::RenderCommand::set_clear_color({ 0.1f, 0.1f, 0.1f, 1 });
		fusion::RenderCommand::clear();

		fusion::Renderer::begin_scene();
		{
			_shader->bind();
			fusion::Renderer::submit(_vertex_array);
		}
		fusion::Renderer::end_scene();
	}
private:
	std::shared_ptr<fusion::VertexArray> _vertex_array;

	std::unique_ptr<fusion::Shader> _shader;
};

class Sandbox : public fusion::Application
{
public:
	Sandbox()
	{
		push_layer(new ExampleLayer());
		push_layer(new RenderLayer());
	}

	~Sandbox()
	{

	}
};

fusion::Application* fusion::create_application() 
{
	return new Sandbox();
}