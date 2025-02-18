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

	glm::vec3 cameraPos;
	glm::vec3 up; 
	glm::vec3 cameraUp;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;

	float FOV_Y;
	float FOV_X;

	float yaw;
	float pitch;
	float roll;

	glm::mat4 get_view_matrix() const;
	glm::mat4 get_proj_matrix() const;

	bool block_in_frustum(const Block& block) const; // checks if block is in view frustum
	bool camera_intersects_block(const Block& block) const; // checks if we're looking at block

	static float get_fov_x_deg(float fov_y);

	void update(); // update camera basis vectors
};