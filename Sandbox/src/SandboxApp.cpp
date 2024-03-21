#include <fusion.h>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

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
	RenderLayer() : Layer("Render"), _camera(-1.6f, 1.6f, -0.9f, 0.9f)
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

			uniform mat4 u_viewProjection;
			
			out vec3 o_color;

			void main() {
				o_color = inColor;
				gl_Position = u_viewProjection * vec4(inPosition, 1.0);
			}
		)";

		const std::string fragmentShader = R"(
			#version 460 core
			layout(location = 0) out vec4 color;

			in vec3 o_color;

			void main() {
				color = vec4(o_color, 1.0);
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

		_camera.SetPosition(_cameraPos);
		_camera.SetRotation(_rotation);

		Fusion::Renderer::BeginScene(_camera);
		{
			Fusion::Renderer::Submit(_shader, _vertexArray);
		}
		Fusion::Renderer::EndScene();
	}

	void OnEvent(Fusion::Event& p_event) override
	{
		if (p_event.GetEventType() == Fusion::EventType::KeyPressed) {
			Fusion::KeyPressedEvent& event = (Fusion::KeyPressedEvent&)p_event;
			if (event.GetKey() == Fusion::Key::Q) 
			{
				_rotation -= 1.0f;
			}
			if (event.GetKey() == Fusion::Key::E)
			{
				_rotation += 1.0f;
			}
		}
		if (p_event.GetEventType() == Fusion::EventType::MouseMoved)
		{
			Fusion::MouseMovedEvent& event = (Fusion::MouseMovedEvent&)p_event;
			float x = event.GetX() / Fusion::Application::Get().GetWindow().GetWidth();
			float y = event.GetY() / Fusion::Application::Get().GetWindow().GetHeight();
			_cameraPos = { (-2.0f * x) + 1.0f, (2.0f * y) - 1.0f, 0.0f};
		}
	}

private:
	std::shared_ptr<Fusion::VertexArray> _vertexArray;
	std::shared_ptr<Fusion::Shader> _shader;
	Fusion::OrthographicCamera _camera;

	float _rotation = 0.0f;
	glm::vec3 _cameraPos = glm::vec3(0.0f);
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