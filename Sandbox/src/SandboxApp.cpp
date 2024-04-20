#include <fusion.h>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Fusion::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate(Fusion::Timestep p_timestep) override
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
		_vertexArray = Fusion::VertexArray::Create();

		float vertices[3 * 6] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		};
		Fusion::Ref<Fusion::VertexBuffer> vertexBuffer;
		vertexBuffer = Fusion::VertexBuffer::Create(vertices, sizeof(vertices));
		{
			Fusion::BufferLayout layout = {
				{ Fusion::ShaderDataType::Float3, "a_position"},
				{ Fusion::ShaderDataType::Float3, "a_color"}
			};

			vertexBuffer->SetLayout(layout);
		}
		_vertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Fusion::Ref<Fusion::IndexBuffer> indexBuffer;
		indexBuffer = Fusion::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		_vertexArray->SetIndexBuffer(indexBuffer);

		_vertexArray->Unbind();


		_squareVertexArray = Fusion::VertexArray::Create();

		float squareVertices[4 * 5] = {
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		};
		Fusion::Ref<Fusion::VertexBuffer> squareVertexBuffer;
		squareVertexBuffer = Fusion::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		{
			Fusion::BufferLayout layout = {
				{ Fusion::ShaderDataType::Float3, "a_position"},
				{ Fusion::ShaderDataType::Float2, "a_uv"}
			};

			squareVertexBuffer->SetLayout(layout);
		}
		_squareVertexArray->AddVertexBuffer(squareVertexBuffer);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Fusion::Ref<Fusion::IndexBuffer> squareIndexBuffer;
		squareIndexBuffer = Fusion::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		_squareVertexArray->SetIndexBuffer(squareIndexBuffer);

		_squareVertexArray->Unbind();

		const std::string vertexShader = R"(
			#version 460 core
			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec3 a_color;

			uniform mat4 u_view_projection;
			uniform mat4 u_transform;
			
			out vec3 v_color;

			void main() {
				v_color = a_color;
				gl_Position = u_view_projection * u_transform * vec4(a_position, 1.0);
			}
		)";

		const std::string fragmentShader = R"(
			#version 460 core
			layout(location = 0) out vec4 color;

			in vec3 v_color;
			uniform vec4 u_color;

			void main() {
				color = u_color;
			}
		)";

		_shader = Fusion::Shader::Create("base", vertexShader, fragmentShader);

		const std::string squareVertexShader = R"(
			#version 460 core
			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec2 a_uv;

			uniform mat4 u_view_projection;
			uniform mat4 u_transform;
			
			out vec2 v_uv;

			void main() {
				v_uv = a_uv;
				gl_Position = u_view_projection * u_transform * vec4(a_position, 1.0);
			}
		)";

		const std::string squareFragmentShader = R"(
			#version 460 core
			layout(location = 0) out vec4 color;

			in vec2 v_uv;
			uniform sampler2D u_texture;

			void main() {
				color = texture(u_texture, v_uv);
			}
		)";
		_squareShader = Fusion::Shader::Create("texture", squareVertexShader, squareFragmentShader);

		_texture = Fusion::Texture2D::Create("assets/models/cottage/textures/cottage_color.png");
		_squareShader->Bind();
		_squareShader->SetInt("u_texture", 0);
	}

	~RenderLayer() 
	{
		_vertexArray->Unbind();
		_squareVertexArray->Unbind();
		_shader->Unbind();
		_squareShader->Unbind();
	}

	void OnUpdate(Fusion::Timestep p_timestep) override
	{
		//FE_TRACE("Delta time: {0}s", p_timestep.GetSeconds());

		if (Fusion::Input::IsKeyPressed(Fusion::Key::Q))
		{
			_rotation -= 60.0f * p_timestep;
		}
		else if (Fusion::Input::IsKeyPressed(Fusion::Key::E))
		{
			_rotation += 60.0f * p_timestep;
		}

		/*float x = Fusion::Input::GetMouseX() / Fusion::Application::Get().GetWindow().GetWidth();
		float y = Fusion::Input::GetMouseY() / Fusion::Application::Get().GetWindow().GetHeight();
		_cameraPos = { (-2.0f * x) + 1.0f, (2.0f * y) - 1.0f, 0.0f };*/

		Fusion::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Fusion::RenderCommand::Clear();

		_camera.SetPosition(_cameraPos);
		_camera.SetRotation(_rotation);

		Fusion::Renderer::BeginScene(_camera);
		{
			_shader->Bind();
			_shader->SetFloat4("u_color", _color);

			static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			for (int y = -10; y < 10; y++)
			{
				for (int x = -10; x < 10; x++)
				{
					glm::vec3 pos = { x * 0.11f, y* 0.11f, 0.0f };
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
					Fusion::Renderer::Submit(_shader, _vertexArray, transform);
				}
			}

			_texture->Bind();
			Fusion::Renderer::Submit(_squareShader, _squareVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		}
		Fusion::Renderer::EndScene();
	}

	void OnImguiRender() override
	{
		ImGui::Begin("test");
		ImGui::Text("Hello world :)");
		ImGui::ColorEdit4("Colors!", glm::value_ptr(_color));
		ImGui::End();
	}

	void OnEvent(Fusion::Event& p_event) override
	{
	}

private:
	Fusion::Ref<Fusion::VertexArray> _vertexArray;
	Fusion::Ref<Fusion::VertexArray> _squareVertexArray;

	Fusion::Ref<Fusion::Shader> _shader;
	Fusion::Ref<Fusion::Shader> _squareShader;
	Fusion::Ref<Fusion::Texture2D> _texture;

	Fusion::OrthographicCamera _camera;

	float _rotation = 0.0f;
	glm::vec3 _cameraPos = glm::vec3(0.0f);
	glm::vec4 _color = glm::vec4(1.0f);
};

class Sandbox : public Fusion::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
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