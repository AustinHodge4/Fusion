#include "fepch.h"
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Fusion {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: _projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), _viewMatrix(1.0f)
	{
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), _position) * 
			glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), {0.0f, 0.0f, 1.0f});

		// Inverse the camera model matrix so that the world moves correctly
		// relative to the camera
		_viewMatrix = glm::inverse(transform);
		// Have to be in P V M order because glm does row major matrices mul
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}
}