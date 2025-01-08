#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Camera.h"
#include "Block.h"

class Game {
public:
	Game(GLFWwindow* window, glm::vec3 cameraStartPos);
	Camera camera;
	ShaderProgram shaderProgram;

	float lastFrame;

	float lastX;
	float lastY;
	bool firstMouse;

	unsigned int texture;

	std::vector<std::vector<std::vector<std::pair<float, float>>>> texCoords;
	std::vector<Block> blocks;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void process_input(GLFWwindow* window);
	void draw();
	void generate_texture();
	int perlin_noise(int x, int y, int maxHeight);
};