#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>
#include "stb_image.h"
#include "BlockType.h"
#include "Constants.h"
#include "Game.h"
#include "Block.h"

Block::Block(ShaderProgram* shaderProgram, std::vector<unsigned int>* VBOs, std::vector<unsigned int>* VAOs, glm::vec3 pos, BlockType blockType) {
	this->game = game;
	this->pos = pos;
	this->shaderProgram = shaderProgram;
	this->blockType = blockType;
	this->VBOs = VBOs;
	this->VAOs = VAOs;

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
	blockIdx = blockToIdx[blockType];

	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
}

void Block::draw() {
	shaderProgram->setMat4("model", model);
	glBindVertexArray(VAOs->at(blockIdx));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}