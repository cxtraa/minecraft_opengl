#pragma once

#include "ShaderProgram.h"

/*
* Crosshair class.
* 
* Renders the crosshair when .draw() is called.
*/

class Crosshair {
public:
	unsigned int VBO, VAO;
	ShaderProgram* shaderProgram;
	Crosshair(ShaderProgram* shaderProgram, float size, float thickness);
	void draw();
};