#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include "Camera.h"
#include "BlockType.h"
#include "CrossHair.h"
#include "Constants.h"
#include "Block.h"
#include "PhysicsSystem.h"
#include "ButtonManager.h"
#include "UIManager.h"
#include "GameState.h"

class Game {
public:
	Game(GLFWwindow* window, glm::vec3 cameraStartPos, bool creative);
	bool creative;
	bool playerOnGround;
	float playerSpeed;

	GameState gameState;
	GLFWwindow* window;
	ShaderProgram crosshairShader; // shader for crosshair
	ShaderProgram worldShader; // shader for blocks
	Camera camera;
	Crosshair crosshair;
	PhysicsSystem physics;
	ButtonManager buttonManager;
	UIManager uiManager;
	
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

	std::vector<Block> blocks;
	std::vector<std::vector<std::vector<bool>>> isBlock;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void game_mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void process_input();
	void draw(); // draw all game objects
	void fill_texture_coords(); // populates `texCoords`
	void generate_texture();
	int get_terrain_height(int x, int z, int maxHeight) const; // returns height of terrain at some (x, z)
	void generate_terrain(); // populates `blocks`
	void gen_vbos_vaos();

	void destroy_block();
	void create_block();

	bool is_visible(const Block& block) const;
	bool collision_occurred(glm::vec3 playerPos);
};