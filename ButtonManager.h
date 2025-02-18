#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

class ButtonManager {
public:
	std::unordered_map<int, bool> prevStates;
	GLFWwindow* window;
	ButtonManager(GLFWwindow* window);
	
	void add_key(int key);
	bool key_single_pressed(int key);
};