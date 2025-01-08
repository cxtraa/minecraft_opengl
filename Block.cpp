#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>
#include "stb_image.h"
#include "BlockType.h"
#include "Constants.h"

#include "Block.h"

Block::Block(ShaderProgram* shaderProgram, glm::vec3 pos, BlockType blockType, std::vector<std::vector<std::vector<std::pair<float, float>>>>& texCoords) {
	this->pos = pos;
	this->shaderProgram = shaderProgram;
	this->blockType = blockType;

	std::unordered_map<BlockType, int> blockToIdx = {
		{OAK_LOG, 0},
		{CHERRY_LEAVES, 1},
		{SNOW, 2},
		{GRAVEL, 3},
		{DIRT, 4},
		{GRASS, 5},
		{SAND, 6},
		{NONE, 7}
	};
	int idx = blockToIdx[blockType];

    float vertices[] = {

        // Bottom face
        -BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][0][0].first, texCoords[idx][0][0].second, // Bottom-left
         BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][0][1].first, texCoords[idx][0][1].second, // Bottom-right
         BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][0][3].first, texCoords[idx][0][3].second, // Top-right

         BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][0][3].first, texCoords[idx][0][3].second, // Top-right
        -BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][0][2].first, texCoords[idx][0][2].second, // Top-left
        -BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][0][0].first, texCoords[idx][0][0].second, // Bottom-left

        // Top face
        -BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][2][0].first, texCoords[idx][2][0].second, // Bottom-left
         BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][2][1].first, texCoords[idx][2][1].second, // Bottom-right
         BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][2][3].first, texCoords[idx][2][3].second, // Top-right

         BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][2][3].first, texCoords[idx][2][3].second, // Top-right
        -BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][2][2].first, texCoords[idx][2][2].second, // Top-left
        -BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][2][0].first, texCoords[idx][2][0].second, // Bottom-left

        // Front face
        -BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left
         BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Bottom-right
         BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right

         BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right
        -BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Top-left
        -BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left

		// Back face
		-BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left
		-BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Top-left
		 BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right

		 BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right
		 BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Bottom-right
		-BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second,  // Bottom-left

        // Left face
        -BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left
        -BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-left
        -BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right

        -BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right
        -BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-right
        -BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left

        // Right face
         BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left
         BLOCK_SIZE/2,  BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-left
         BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right

         BLOCK_SIZE/2, -BLOCK_SIZE/2, -BLOCK_SIZE/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right
         BLOCK_SIZE/2, -BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-right
         BLOCK_SIZE/2,  BLOCK_SIZE/2,  BLOCK_SIZE/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second  // Bottom-left
    };

	// VBO, VAO
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
}

void Block::draw() {
	shaderProgram->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}