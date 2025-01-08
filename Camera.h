#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Block.h"

/*
* Camera class.
* Holds world up vector, and camera basis vectors.
* Update using .update();
*/

class Camera {
public:
	Camera(glm::vec3 startPos);

	glm::vec3 up; // world up vector

	// Camera basis vectors
	glm::vec3 cameraUp;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;

	glm::vec3 cameraPos;

	float yaw;
	float pitch;
	float roll;

	float FOV_Y;
	float FOV_X;

	glm::mat4 get_view_matrix();
	glm::mat4 get_proj_matrix();

	bool block_in_frustum(Block& block); // checks if block is in view frustum
	bool camera_intersects_block(Block& block); // checks if we're looking at block

	float get_fov_x_deg(float fov_y);

	void update(); // update camera basis vectors
};