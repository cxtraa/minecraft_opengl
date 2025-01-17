#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "BlockType.h"
#include "ShaderProgram.h"

/*
* Drawing a block creates 1 cube with the specified block type.
* 
* Has its own VAO and VBO and a reference to shader program.
* Stores its own model matrix.
*/

class Game;

class Block {
public:
	Game* game;
	ShaderProgram* shaderProgram;
	std::vector<unsigned int>* VBOs;
	std::vector<unsigned int>* VAOs;
	int blockIdx;
	BlockType blockType;
	glm::vec3 pos;
	glm::mat4 model; // model matrix (local -> world space)

	int i;
	int j;
	int k;

	Block(ShaderProgram* shaderProgram, std::vector<unsigned int>* VBOs, std::vector<unsigned int>* VAOs, int i, int j, int k, BlockType blockType);

	void draw();
};
