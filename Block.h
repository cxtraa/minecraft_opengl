#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "BlockType.h"
#include "ShaderProgram.h"

/*
* The Block class. Fundamental building piece of the game.
* 
* Contains pre-defined vertices (not set by user).
* User can specify texture type.
* 
*/

class Block {
public:
	ShaderProgram* shaderProgram;
	unsigned int VAO;
	unsigned int VBO;
	BlockType blockType;
	glm::vec3 pos;
	glm::mat4 model;

	Block(ShaderProgram* shaderProgram, glm::vec3 pos, BlockType blockType, std::vector<std::vector<std::vector<std::pair<float, float>>>>& texCoords);

	void draw();
	void update();
};
