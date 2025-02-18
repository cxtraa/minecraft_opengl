#pragma once

// Stores current velocity and acceleration due to external forces on player
// We will add this contribution to velocity from player input

#include <glm/glm.hpp>
#include <vector>

class PhysicsSystem {
public:

	glm::vec3 v;
	glm::vec3 a;
	glm::vec3 r;
	float lastTime;

	float get_delta_time();
	glm::vec3 get_delta_r();

	PhysicsSystem(glm::vec3 v0, glm::vec3 a0, glm::vec3 r0);
};