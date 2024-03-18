#include <fusion.h>

#include <imgui.h>

class ExampleLayer : public Fusion::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		if (Fusion::Input::IsKeyPressed(Fusion::Key::W))
		{
			FE_TRACE("Pressed W key (poll)");
		}
		
	}

	void OnImguiRender() override
	{
		ImGui::Begin("test");
		ImGui::Text("Hello world :)");
		ImGui::End();
	}

	void OnEvent(Fusion::Event& p_event) override
	{
		if (p_event.GetEventType() == Fusion::EventType::KeyPressed) {
			Fusion::KeyPressedEvent& event = (Fusion::KeyPressedEvent&)p_event;
			if (event.GetKey() == Fusion::Key::W) {
				FE_TRACE("Pressed {0} key (event)!", Fusion::Key::GetKeyDetails(Fusion::Key::W)->GetDisplayName());
			}
			FE_TRACE("{0}", event.GetKey().GetName());
		}
	}
};

class RenderLayer : public Fusion::Layer
{
public:
	RenderLayer() : Layer("Render") 
	{
		_vertexArray.reset(Fusion::VertexArray::Create());

		float vertices[3 * 6] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		};
		std::shared_ptr<Fusion::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Fusion::VertexBuffer::Create(vertices, sizeof(vertices)));
		{
			Fusion::BufferLayout layout = {
				{ Fusion::ShaderDataType::Float3, "inPosition"},
				{ Fusion::ShaderDataType::Float3, "inColor"}
			};

			vertexBuffer->SetLayout(layout);
		}
		_vertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Fusion::IndexBuffer> indexBuffer;
		indexBuffer.reset(Fusion::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		_vertexArray->SetIndexBuffer(indexBuffer);

		_vertexArray->Unbind();

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

		_shader.reset(Fusion::Shader::Create("base", vertexShader, fragmentShader));
	}

	~RenderLayer() 
	{
		_vertexArray->Unbind();
		_shader->Unbind();
	}

	void OnUpdate() override
	{
		Fusion::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Fusion::RenderCommand::Clear();

		Fusion::Renderer::BeginScene();
		{
			_shader->Bind();
			Fusion::Renderer::Submit(_vertexArray);
		}
		Fusion::Renderer::EndScene();
	}
private:
	std::shared_ptr<Fusion::VertexArray> _vertexArray;

	std::unique_ptr<Fusion::Shader> _shader;
};

class Sandbox : public Fusion::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushLayer(new RenderLayer());
	}

	~Sandbox()
	{

	}
};

Fusion::Application* Fusion::CreateApp() 
{
	return new Sandbox();
}