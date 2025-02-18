#include "Crosshair.h"
#include "Constants.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Crosshair::Crosshair(ShaderProgram* shaderProgram, float size, float thickness) :
	shaderProgram(shaderProgram)
{
	float vertices[] = {
		// Horizontal rectangle
		-size / SCREEN_WIDTH, thickness / SCREEN_HEIGHT,   // Bottom left
		size / SCREEN_WIDTH, thickness / SCREEN_HEIGHT,   // Bottom right
		size / SCREEN_WIDTH, -thickness / SCREEN_HEIGHT,  // Top right
		-size / SCREEN_WIDTH, -thickness / SCREEN_HEIGHT,  // Top left

		// Vertical rectangle
		-thickness / SCREEN_WIDTH, size / SCREEN_HEIGHT,   // Bottom left
		thickness / SCREEN_WIDTH, size / SCREEN_HEIGHT,   // Bottom right
		thickness / SCREEN_WIDTH, -size / SCREEN_HEIGHT,  // Top right
		-thickness / SCREEN_WIDTH, -size / SCREEN_HEIGHT   // Top left
	};

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Crosshair::draw() {
	shaderProgram->use();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glBindVertexArray(0);
}