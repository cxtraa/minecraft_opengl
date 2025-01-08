#include "Game.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Constants.h"
#include "BlockType.h"
#include "stb_image.h"#
#include "CornerLocation.h"

#include <unordered_map>
#include <vector>
#include <iostream>

Game::Game(GLFWwindow* window, glm::vec3 cameraStartPos) :
	camera(cameraStartPos),
	shaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetWindowUserPointer(window, this);
	generate_texture();

	lastFrame = 0.0f;

	lastX = screenWidth / 2;
	lastY = screenHeight / 2;

	firstMouse = true;

	// Array for tex coordinate positions.
	// For position (i, j), the bottom left is located at (j*texW, i*texH).
	
	texCoords = std::vector<std::vector<std::vector<std::pair<float, float>>>>(numTexturesY, std::vector<std::vector<std::pair<float,float>>>(numTexturesX, std::vector<std::pair<float,float>>(4, {0.0f, 0.0f})));
	for (unsigned int i = 0; i < numTexturesY; i++) {
		for (unsigned int j = 0; j < numTexturesX; j++) {
			texCoords[i][j][0] = { (j+halfPixel) * texW, (i+halfPixel) * texH }; // Bottom left
			texCoords[i][j][1] = { (j+1-halfPixel) * texW, (i+halfPixel) * texH }; // Bottom right
			texCoords[i][j][2] = { (j+halfPixel) * texW, (i+1-halfPixel) * texH }; // Top left
			texCoords[i][j][3] = { (j+1-halfPixel) * texW, (i+1-halfPixel) * texH }; // Top right
		}
	}

	// Create a grid of blocks
	int maxHeight = 20;
	for (unsigned int i = 0; i < 50; i++) {
		for (unsigned int k = 0; k < 50; k++) {
			int height = perlin_noise(i, k, maxHeight);
			for (int j = 0; j < height; j++) {
				Block block(&shaderProgram, glm::vec3(i * blockSize, j * blockSize, k * blockSize), OAK_LOG, texCoords);
				blocks.push_back(block);
			}
		}
	}
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Game::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Mouse callback goes here.

	// Get instance of Game (pointer to it).
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

	if (game->firstMouse) {
		game->lastX = xpos;
		game->lastY = ypos;
		game->firstMouse = false;
	}

	float xoffset = (xpos - game->lastX) * sensitivity;
	float yoffset = -(ypos - game->lastY) * sensitivity;
	game->lastX = xpos;
	game->lastY = ypos;

	game->camera.yaw += xoffset;
	game->camera.pitch += yoffset;

	game->camera.yaw = glm::mod(game->camera.yaw, 360.0f);

	// Keep pitch in -89.0f - 89.0f
	if (game->camera.pitch > 89.0f) {
		game->camera.pitch = 89.0f;
	}
	else if (game->camera.pitch < -89.0f) {
		game->camera.pitch = -89.0f;
	}
}

void Game::process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	float currentTime = glfwGetTime();
	float deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	// Movement controls
	glm::vec3 planeUnitVector = camera.cameraFront - glm::dot(camera.cameraFront, camera.cameraUp) * camera.cameraUp;
	planeUnitVector = glm::normalize(planeUnitVector);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.cameraPos += planeUnitVector * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.cameraPos -= glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp)) * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.cameraPos -= planeUnitVector * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.cameraPos += glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp)) * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.cameraPos += camera.cameraUp * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.cameraPos -= camera.cameraUp * PLAYER_SPEED * deltaTime;
	}
}

void Game::draw() {
	for (Block& block : blocks) {
		block.draw();
	}
}

void Game::generate_texture() {
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(TEXTURE_PATH, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Texture image failed to load." << std::endl;
	}
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

int Game::perlin_noise(int x, int y, int maxHeight) {
	return (int)(maxHeight * (sin(((float)x + (float)y) / 10) + 1.1)/2);
}