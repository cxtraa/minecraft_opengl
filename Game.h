#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>
#include "Camera.h"
#include "BlockType.h"
#include "CrossHair.h"
#include "Constants.h"
#include "Block.h"

class Game {
public:
	Game(GLFWwindow* window, glm::vec3 cameraStartPos);
	Camera camera;
	Crosshair crosshair;
	ShaderProgram worldShader; // shader for blocks
	ShaderProgram crosshairShader; // shader for crosshair

	float lastFrame; // time since last frame for deltaTime calcs
	float lastClickEventTime; // time since last block placed / destroyed

	float lastMousePosX;
	float lastMousePosY;
	bool isFirstMouse; // is this first mouse movement?

	unsigned int texture; // ID for texture atlas of all blocks

	// texCoords[i][j][cornerIndex] gives (x, y) coords on texture atlas for texture at index [i][j]. [0][0] is bottom left of texture atlas.
	// see fill_texture_coords() for cornerIndex assignments
	std::vector<std::vector<std::vector<std::pair<float, float>>>> texCoords; 

	std::vector<unsigned int> VBOs;
	std::vector<unsigned int> VAOs;

	BlockType blockToPlace;
	std::unordered_map<int, BlockType> blockPlaceKeyBinds; // user presses number to change block to place
	std::vector<Block> blocks; // stores all blocks in world

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

	void process_input(GLFWwindow* window);
	void draw(); // draw all game objects
	void fill_texture_coords(); // populates `texCoords`
	void generate_texture();
	int get_terrain_height(int x, int z, int maxHeight); // returns height of terrain at some (x, z)
	void generate_terrain(); // populates `blocks`
	void gen_vbos_vaos();

	void destroy_block();
	void create_block();
};