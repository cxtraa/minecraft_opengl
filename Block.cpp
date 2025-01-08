#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "stb_image.h"
#include "BlockType.h"
#include "Constants.h"
#include "CornerLocation.h"

#include "Block.h"

Block::Block(ShaderProgram* shaderProgram, glm::vec3 pos, BlockType blockType, std::vector<std::vector<std::vector<std::pair<float, float>>>>& texCoords) {
	this->pos = pos;
	this->shaderProgram = shaderProgram;
	this->blockType = blockType;

	int idx = 0;
	if (blockType == OAK_LOG) {
		idx = 0;
	}
	else if (blockType == CHERRY_LEAVES) {
		idx = 1;
	}
	else if (blockType == SNOW) {
		idx = 2;
	}
	else if (blockType == GRAVEL) {
		idx = 3;
	}
	else if (blockType == DIRT) {
		idx = 4;
	}
	else if (blockType == GRASS) {
		idx = 5;
	}
	else if (blockType == SAND) {
		idx = 6;
	}
	else if (blockType == NONE) {
		idx = 7;
	}

    float vertices[] = {
        // Positions                    // Texture Coordinates (U, V)

        // Bottom face (0)
        -blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][0][0].first, texCoords[idx][0][0].second, // Bottom-left
         blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][0][1].first, texCoords[idx][0][1].second, // Bottom-right
         blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][0][3].first, texCoords[idx][0][3].second, // Top-right

         blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][0][3].first, texCoords[idx][0][3].second, // Top-right
        -blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][0][2].first, texCoords[idx][0][2].second, // Top-left
        -blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][0][0].first, texCoords[idx][0][0].second, // Bottom-left

        // Top face (2)
        -blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][2][0].first, texCoords[idx][2][0].second, // Bottom-left
         blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][2][1].first, texCoords[idx][2][1].second, // Bottom-right
         blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][2][3].first, texCoords[idx][2][3].second, // Top-right

         blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][2][3].first, texCoords[idx][2][3].second, // Top-right
        -blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][2][2].first, texCoords[idx][2][2].second, // Top-left
        -blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][2][0].first, texCoords[idx][2][0].second, // Bottom-left

        // Front face (1)
        -blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left
         blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Bottom-right
         blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right

         blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right
        -blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Top-left
        -blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left

		// Back face (5)
		-blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left
		-blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Top-left
		 blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right

		 blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right
		 blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Bottom-right
		-blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second,  // Bottom-left

        // Left face (2)
        -blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left
        -blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-left
        -blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right

        -blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right
        -blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-right
        -blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left

        // Right face (3)
         blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left
         blockSize/2,  blockSize/2, -blockSize/2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-left
         blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right

         blockSize/2, -blockSize/2, -blockSize/2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right
         blockSize/2, -blockSize/2,  blockSize/2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-right
         blockSize/2,  blockSize/2,  blockSize/2, texCoords[idx][1][2].first, texCoords[idx][1][2].second  // Bottom-left
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
	glBindVertexArray(VAO);
	shaderProgram->setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Block::update() {

}