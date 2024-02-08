#include "fepch.h"
#include "imgui_layer.h"

#include "imgui.h"

#include "platform/opengl/imgui_impl_opengl3.h"

// Temporary
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fusion/application.h"

namespace fusion {

	ImGUILayer::ImGUILayer() : Layer("imgui")
	{
	}

	ImGUILayer::~ImGUILayer()
	{

	}

	void ImGUILayer::on_attach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// [Temporary]
		// Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array.
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGUILayer::on_detach()
	{

	}

	void ImGUILayer::on_update()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2(app.get_window().get_width(), app.get_window().get_height());

		float time = (float)glfwGetTime();
		io.DeltaTime = _time > 0.0f ? (time - _time) : (1.0f / 60.0f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGUILayer::on_event(Event& e)
	{
		//FE_CORE_INFO("imgui {0}", e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(FE_BIND_EVENT_FN(on_mouse_scrolled_event));
		dispatcher.Dispatch<MouseButtonPressedEvent>(FE_BIND_EVENT_FN(on_mouse_button_press_event));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(FE_BIND_EVENT_FN(on_mouse_button_released_event));
		dispatcher.Dispatch<MouseMovedEvent>(FE_BIND_EVENT_FN(on_mouse_moved_event));
		dispatcher.Dispatch<KeyPressedEvent>(FE_BIND_EVENT_FN(on_key_pressed_event));
		dispatcher.Dispatch<KeyReleasedEvent>(FE_BIND_EVENT_FN(on_key_released_event));
		dispatcher.Dispatch<KeyTypedEvent>(FE_BIND_EVENT_FN(on_key_typed_event));
		dispatcher.Dispatch<WindowResizeEvent>(FE_BIND_EVENT_FN(on_window_resized_event));
	}

	bool ImGUILayer::on_mouse_scrolled_event(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.get_offset_x();
		io.MouseWheel += e.get_offset_y();
		return false;
	}

	bool ImGUILayer::on_mouse_button_press_event(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.get_mouse_button()] = true;
		return false;
	}

	bool ImGUILayer::on_mouse_button_released_event(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.get_mouse_button()] = false;
		return false;
	}

	bool ImGUILayer::on_mouse_moved_event(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.get_x(), e.get_y());
		return false;
	}
	bool ImGUILayer::on_key_pressed_event(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.get_keycode()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		return false;
	}
	bool ImGUILayer::on_key_released_event(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.get_keycode()] = false;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		return false;
	}
	bool ImGUILayer::on_key_typed_event(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(e.get_keycode());

		return false;
	}
	bool ImGUILayer::on_window_resized_event(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.get_width(), e.get_height());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.get_width(), e.get_height());

		return false;
	}
}