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
#include <iostream>

Block::Block(ShaderProgram* shaderProgram, std::vector<unsigned int>* VBOs, std::vector<unsigned int>* VAOs, int i, int j, int k, BlockType blockType) :
	game(game),
	shaderProgram(shaderProgram),
	blockType(blockType),
	VBOs(VBOs),
	VAOs(VAOs),
	i(i), j(j), k(k),
	pos(glm::vec3((float)i* BLOCK_SIZE, (float)j* BLOCK_SIZE, (float)k* BLOCK_SIZE)),
	blockIdx(blockToIdx.at(blockType)),
	model(glm::translate(glm::mat4(1.0f), pos))
{}

void Block::draw() {
	shaderProgram->setMat4("model", model);
	glBindVertexArray(VAOs->at(blockIdx));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}