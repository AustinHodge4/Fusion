#pragma once

#include <glm/glm.hpp>

namespace Fusion {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		inline const glm::vec3& GetPosition() const { return _position; }
		inline void SetPosition(const glm::vec3& p_position) { _position = p_position; RecalculateViewMatrix(); }

		inline float GetRotation() const { return _rotation; }
		inline void SetRotation(float p_rotation) { _rotation = p_rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return _projectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return _viewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return _viewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjectionMatrix;

		glm::vec3 _position = glm::vec3(0.0f);
		float _rotation = 0.0f;
	};
}