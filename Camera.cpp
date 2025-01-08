
#include <glm/glm.hpp>
#include "Camera.h"

Camera::Camera(glm::vec3 startPos) {
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraPos = startPos;
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	yaw = -90.0f;
	pitch = 0.0f;
	roll = 0.0f;
}

glm::mat4 Camera::get_view_matrix() {
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}