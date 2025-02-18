#include "PhysicsSystem.h"
#include <GLFW/glfw3.h>

PhysicsSystem::PhysicsSystem(glm::vec3 v0, glm::vec3 a0, glm::vec3 r0) :
	v(v0),
	a(a0),
	r(r0),
	lastTime(0)
{}

float PhysicsSystem::get_delta_time() {
	float delta_time = glfwGetTime() - lastTime;
	lastTime = glfwGetTime();
	return delta_time;
}

glm::vec3 PhysicsSystem::get_delta_r() {
	float dt = get_delta_time();
	v += a * dt; // Find new velocity
	glm::vec3 delta_r = v * dt;
	return delta_r;
}