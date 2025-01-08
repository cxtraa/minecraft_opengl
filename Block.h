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

class Block {
public:
	ShaderProgram* shaderProgram;
	unsigned int VAO;
	unsigned int VBO;
	BlockType blockType;
	glm::vec3 pos;
	glm::mat4 model; // model matrix (local -> world space)

	Block(ShaderProgram* shaderProgram, glm::vec3 pos, BlockType blockType, std::vector<std::vector<std::vector<std::pair<float, float>>>>& texCoords);

	void draw();
};
