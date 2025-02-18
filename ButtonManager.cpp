
#include "ButtonManager.h"

ButtonManager::ButtonManager(GLFWwindow* window) :
	window(window) 
{}

void ButtonManager::add_key(int key) {
	prevStates[key] = glfwGetKey(window, key) == GLFW_PRESS;
}

bool ButtonManager::key_single_pressed(int key) {
	bool currentState = glfwGetKey(window, key) == GLFW_PRESS;
	bool prevState = prevStates.at(key);
	prevStates.at(key) = currentState;
	return currentState && !prevState;
}