
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "Camera.h"
#include "Constants.h"

Camera::Camera(glm::vec3 startPos) {
	cameraPos = startPos;
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

	FOV_Y = DEFAULT_FOV_Y;
	FOV_X = get_fov_x_deg(FOV_Y);
	
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
}

glm::mat4 Camera::get_view_matrix() {
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
	return view;
}

glm::mat4 Camera::get_proj_matrix() {
	glm::mat4 proj = glm::perspective(glm::radians(FOV_Y), ASPECT_RATIO, NEAR, FAR);
	return proj;
}

void Camera::update() {
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	cameraRight = glm::normalize(glm::cross(cameraFront, up));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

bool Camera::block_in_frustum(Block& block) {
	// Determine if `block` is inside the view frustum.
	// This is done by considering projections of the vector from camera to block in x, y, z directions.
	// The bounds for the view frustum in x, y, z, can be found by trigonometry.

	glm::vec3 block_pos = block.pos - cameraPos; // Vector from camera to block

	// NEAR and FAR
	float proj_z = glm::dot(block_pos, cameraFront);
	if (!(NEAR - BLOCK_RADIUS <= proj_z && proj_z <= FAR + BLOCK_RADIUS)) return false;

	// TOP and BOTTOM
	float proj_y = glm::dot(block_pos, cameraUp);
	float dist_y = BLOCK_RADIUS / std::cos(glm::radians(FOV_Y) * 0.5f) + proj_z * std::tan(glm::radians(FOV_Y) * 0.5f);
	if (!(-dist_y <= proj_y && proj_y <= dist_y)) return false;

	// LEFT and RIGHT
	float proj_x = glm::dot(block_pos, cameraRight);
	float dist_x = BLOCK_RADIUS / std::cos(glm::radians(FOV_X) * 0.5f) + proj_z * std::tan(glm::radians(FOV_X) * 0.5f);;
	if (!(-dist_x <= proj_x && proj_x <= dist_x)) return false;


	return true;
}

bool Camera::camera_intersects_block(Block& block) {
	// Determine if the player is looking at `block`.
	// This is done by solving the intersection of a line (cameraFront vec) and a sphere (block).

	glm::vec3 ac = cameraPos - block.pos;
	float B = glm::dot(2 * ac, cameraFront);
	float C = glm::length2(ac) - pow(BLOCK_RADIUS, 2);
	return pow(B, 2) - 4 * C >= 0; // The discriminant should be >= 0 for real solutions
}

float Camera::get_fov_x_deg(float fov_y) {
	return glm::degrees(2.0f * glm::atan(ASPECT_RATIO * glm::tan(glm::radians(fov_y) / 2.0f)));
}