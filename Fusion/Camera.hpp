#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	float fov;
	float speed;
	float mouseSensitivity;
	float zoom;

	Camera(glm::vec3 position, glm::vec3 up, float speed, float fov) : forward(glm::vec3(0.0f, 0.0f, -1.0f)), mouseSensitivity(0.01f), zoom(45.0f) {
		this->position = position;
		this->worldUp = up;

		this->speed = speed;
		this->fov = fov;

		if (worldUp == glm::vec3(0.0f, 0.0f, 1.0f)) {
			this->forward = glm::vec3(0.0f, -1.0f, 0.0f);
		}

		updateCameraVectors();
	}
	Camera() {};

	void move(int key, float deltaTime) {
		float velocity = speed * deltaTime;
		if (key == GLFW_KEY_UP)
			position += forward * velocity;
		if (key == GLFW_KEY_DOWN)
			position -= forward * velocity;
		if (key == GLFW_KEY_RIGHT)
			position += right * velocity;
		if (key == GLFW_KEY_LEFT)
			position -= right * velocity;
	}

	void look(double xoffset, double yoffset)
	{
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		yaw += xoffset;
		pitch += yoffset;

		pitch = glm::clamp(pitch, -89.0f, 89.0f);

		updateCameraVectors();
	}
	void process_zoom(float yoffset)
	{
		if (zoom >= 1.0f && zoom <= 100.0f)
			zoom -= yoffset;
		zoom = glm::clamp(zoom, 1.0f, 100.0f);
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + forward, up);
	}
private:
	glm::vec3 position;
	glm::vec3 right;
	glm::vec3 forward;
	glm::vec3 worldUp;
	glm::vec3 up;

	float yaw = 180.0f;
	float pitch = 0.0f;

	void updateCameraVectors() {
		
		if (worldUp == glm::vec3(0.0f, 0.0f, 1.0f)) {
			forward.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			forward.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			forward.z = sin(glm::radians(pitch));
		}
		else {
			forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			forward.y = sin(glm::radians(pitch));
			forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		}

		forward = glm::normalize(forward);
		right = glm::normalize(glm::cross(forward, worldUp));
		up = glm::normalize(glm::cross(right, forward));
	}
};