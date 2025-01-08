#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/*
* Interface for creating shader programs.
* 
* Takes in filepath to vertex shader and fragment shader.
* 
* Allows updating of uniforms via various "set" methods.
*/

class ShaderProgram {
public:
	unsigned int ID;
	ShaderProgram(const char* const vertexPath, const char* const fragmentPath);
	void use();
	void setBool(const std::string& uniformName, bool value) const;
	void setInt(const std::string& uniformName, int value) const;
	void setFloat(const std::string& uniformName, float value) const;
	void setVec4(const std::string& uniformName, glm::vec4 value) const;
	void setMat4(const std::string& uniformName, glm::mat4 value) const;
	void checkCompileErrors(unsigned int shader, const std::string& type);
};