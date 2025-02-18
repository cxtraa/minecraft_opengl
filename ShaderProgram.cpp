#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram(const char* const vertexPath, const char* const fragmentPath) {

	std::ifstream vertexFile(vertexPath);
	if (!vertexFile.is_open()) {
		std::cerr << "ERROR::SHADER::VERTEX::FILE_NOT_SUCCESSFULLY_READ: " << vertexPath << "\n";
	}
	std::ifstream fragmentFile(fragmentPath);
	if (!fragmentFile.is_open()) {
		std::cerr << "ERROR::SHADER::FRAGMENT::FILE_NOT_SUCCESSFULLY_READ: " << fragmentPath << "\n";
	}

	std::stringstream vertexStream, fragmentStream;
	vertexStream << vertexFile.rdbuf();
	fragmentStream << fragmentFile.rdbuf();
	vertexFile.close();
	fragmentFile.close();

	std::string vertexShaderStr = vertexStream.str();
	std::string fragmentShaderStr = fragmentStream.str();

	const char* vertexShader = vertexShaderStr.c_str();
	const char* fragmentShader = fragmentShaderStr.c_str();

	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShader, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShader, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void ShaderProgram::use() {
	glUseProgram(ID);
}

void ShaderProgram::setBool(const std::string& uniformName, bool value) const {
	glUniform1i(glGetUniformLocation(ID, uniformName.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& uniformName, int value) const {
	glUniform1i(glGetUniformLocation(ID, uniformName.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& uniformName, float value) const {
	glUniform1f(glGetUniformLocation(ID, uniformName.c_str()), value);
}

void ShaderProgram::setVec4(const std::string& uniformName, glm::vec4 value) const {
	glUniform4fv(glGetUniformLocation(ID, uniformName.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const std::string& uniformName, glm::mat4 value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::checkCompileErrors(unsigned int shader, const std::string& type) const {
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
		}
	}
}
