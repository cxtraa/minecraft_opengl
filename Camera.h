#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera {
public:
	Camera(glm::vec3 startPos);
	glm::mat4 get_view_matrix();
	glm::vec3 cameraUp;
	glm::vec3 cameraFront;
	glm::vec3 cameraPos;

	float yaw;
	float pitch;
	float roll;
};