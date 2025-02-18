#include "Game.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Constants.h"
#include "BlockType.h"
#include "stb_image.h"
#include "UIManager.h"
#include "GameState.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <unordered_map>
#include <vector>
#include <iostream>

Game::Game(GLFWwindow* window, glm::vec3 cameraStartPos, bool creative) :
	creative(creative),
	playerOnGround(false),
	playerSpeed(PLAYER_SPEED),
	gameState(InGame),
	window(window),
	crosshairShader(CROSSHAIR_VERTEX_SHADER_PATH, CROSSHAIR_FRAGMENT_SHADER_PATH),
	worldShader(WORLD_VERTEX_SHADER_PATH, WORLD_FRAGMENT_SHADER_PATH),
	camera(cameraStartPos),
	crosshair(&crosshairShader, CROSSHAIR_SIZE, CROSSHAIR_THICKNESS),
	physics(glm::vec3(0.0f), creative ? glm::vec3(0.0f) : glm::vec3(0.0f, -9.8f, 0.0f), cameraStartPos),
	buttonManager(window),
	uiManager(window, this),
	lastFrame(0.0f),
	lastClickEventTime(0.0f),
	lastMousePosX(SCREEN_WIDTH / 2),
	lastMousePosY(SCREEN_HEIGHT / 2),
	isFirstMouse(true),
	isBlock(WORLD_MAX_X, std::vector<std::vector<bool>>(WORLD_MAX_Y, std::vector<bool>(WORLD_MAX_Z, false))),
	VBOs(numBlockTypes, 0),
	VAOs(numBlockTypes, 0),
	blockToPlace(DIRT)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, game_mouse_callback);
	glfwSetWindowUserPointer(window, this);

	buttonManager.add_key(GLFW_KEY_C);
	buttonManager.add_key(GLFW_KEY_ESCAPE);

	generate_texture();
	texCoords = std::vector<std::vector<std::vector<std::pair<float, float>>>>(numTexturesY, std::vector<std::vector<std::pair<float,float>>>(numTexturesX, std::vector<std::pair<float,float>>(4, {0.0f, 0.0f})));
	fill_texture_coords();
	gen_vbos_vaos();
	generate_terrain();
}

void Game::draw() {
	glEnable(GL_DEPTH_TEST); // Depth testing should be on for the blocks
	worldShader.use();
	camera.update();
	worldShader.setMat4("view", camera.get_view_matrix());
	worldShader.setMat4("proj", camera.get_proj_matrix());
	for (Block& block : blocks) {
		if (camera.block_in_frustum(block) && is_visible(block)) {
			block.draw();
		}
	}
	glDisable(GL_DEPTH_TEST); // To ensure crosshair is on top, turn off depth test
	crosshair.draw();
	uiManager.draw();
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Game::game_mouse_callback(GLFWwindow* window, double currMousePosX, double currMousePosY) {

	// Get instance of Game (pointer to it).
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

	if (game->isFirstMouse) {
		game->lastMousePosX = currMousePosX;
		game->lastMousePosY = currMousePosY;
		game->isFirstMouse = false;
	}

	float xoffset = (currMousePosX - game->lastMousePosX) * SENSITIVITY;
	float yoffset = -(currMousePosY - game->lastMousePosY) * SENSITIVITY;
	game->lastMousePosX = currMousePosX;
	game->lastMousePosY = currMousePosY;

	game->camera.yaw += xoffset;
	game->camera.pitch += yoffset;

	game->camera.yaw = glm::mod(game->camera.yaw, 360.0f); // Every 360 deg rotation leaves yaw unchanged

	// Keep pitch in -89.0f - 89.0f
	if (game->camera.pitch > 89.0f) {
		game->camera.pitch = 89.0f;
	}
	else if (game->camera.pitch < -89.0f) {
		game->camera.pitch = -89.0f;
	}

	ImGui_ImplGlfw_CursorPosCallback(window, currMousePosX, currMousePosY);
}

void Game::process_input() {

	if (buttonManager.key_single_pressed(GLFW_KEY_ESCAPE)) {
		gameState == InGame ? uiManager.trans_to_settings() : uiManager.trans_to_game();
	}
	if (gameState == Settings) return;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		playerSpeed = 2 * PLAYER_SPEED;
	}
	else {
		playerSpeed = PLAYER_SPEED;
	}
	glm::vec3 planeUnitVector = camera.cameraFront - glm::dot(camera.cameraFront, camera.up) * camera.up;
	planeUnitVector = glm::normalize(planeUnitVector);

	glm::vec3 input_velocity = glm::vec3(0.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		input_velocity += planeUnitVector * playerSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		input_velocity -= glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp)) * playerSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		input_velocity -= planeUnitVector * playerSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		input_velocity += camera.cameraRight * playerSpeed;
	}

	if (creative) {
		physics.a = glm::vec3(0.0f);
		physics.v = glm::vec3(0.0f);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) {
			input_velocity += camera.up * playerSpeed;	
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			input_velocity -= camera.up * playerSpeed;
		}
	}
	else {
		physics.a = glm::vec3(0.0f, WORLD_GRAVITY, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && playerOnGround && physics.v.y == 0) {
			physics.v += glm::vec3(0.0f, 1.0f, 0.0f) * JUMP_SPEED;
			playerOnGround = false;
		}
	}

	physics.v += input_velocity;
	glm::vec3 delta = physics.get_delta_r();
	physics.v -= input_velocity;

	glm::vec3 new_pos = camera.cameraPos;
	if (!collision_occurred(camera.cameraPos + glm::vec3(delta.x, 0.0f, 0.0f))) {
		new_pos += glm::vec3(delta.x, 0.0f, 0.0f);
	}
	else {
		physics.v.x = 0.0f;
	}
	if (!collision_occurred(camera.cameraPos + glm::vec3(0.0f, delta.y, 0.0f))) {
		new_pos += glm::vec3(0.0f, delta.y, 0.0f);
	}
	else {
		physics.v.y = 0.0f;
	}
	if (!collision_occurred(camera.cameraPos + glm::vec3(0.0f, 0.0f, delta.z))) {
		
		new_pos += glm::vec3(0.0f, 0.0f, delta.z);
	}
	else {
		physics.v.z = 0.0f;
	}
	camera.cameraPos = new_pos;
	
	if (glfwGetTime() - lastClickEventTime >= CLICK_COOLDOWN_TIME && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		lastClickEventTime = glfwGetTime();
		destroy_block();
	}
	if (glfwGetTime() - lastClickEventTime >= CLICK_COOLDOWN_TIME && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		lastClickEventTime = glfwGetTime();
		create_block();
	}

	// Switch block selection to create
	for (auto it = keyToBlock.begin(); it != keyToBlock.end(); it++) {
		if (glfwGetKey(window, it->first) == GLFW_PRESS) {
			blockToPlace = it->second;
		}
	}

	// Check for switch to creative
	if (buttonManager.key_single_pressed(GLFW_KEY_C)) {
		creative = !creative;
	}
}

void Game::fill_texture_coords() {
	for (unsigned int i = 0; i < numTexturesY; i++) {
		for (unsigned int j = 0; j < numTexturesX; j++) {
			texCoords[i][j][0] = { (j + halfPixel) * texW, (i + halfPixel) * texH }; // Bottom left
			texCoords[i][j][1] = { (j + 1 - halfPixel) * texW, (i + halfPixel) * texH }; // Bottom right
			texCoords[i][j][2] = { (j + halfPixel) * texW, (i + 1 - halfPixel) * texH }; // Top left
			texCoords[i][j][3] = { (j + 1 - halfPixel) * texW, (i + 1 - halfPixel) * texH }; // Top right
		}
	}
}

void Game::generate_texture() {
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // Images are inverted by default
	unsigned char* data = stbi_load(TEXTURE_PATH, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Texture image failed to load." << std::endl;
	}
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

int Game::get_terrain_height(int x, int z, int maxHeight) const {
	return (int)(maxHeight * (0.5f + 0.5f * cos((float)x/15.0f) * cos((float)z/15.0f))) % maxHeight;
}

void Game::generate_terrain() {
	for (int i = 0; i < WORLD_MAX_X; i++) {
		for (int k = 0; k < WORLD_MAX_Z; k++) {
			int height = get_terrain_height(i, k, WORLD_MAX_Y / 2);
			for (int j = 0; j < height; j++) {
				Block block(&worldShader, &VBOs, &VAOs, i, j, k, DIRT);
				isBlock[i][j][k] = true;
				blocks.push_back(block);
			}
			Block block(&worldShader, &VBOs, &VAOs, i, height, k, OAK_LOG);
			isBlock[i][height][k] = true;
			blocks.push_back(block);
		}
	}
}

void Game::destroy_block() {
	// Iterate through all blocks to find closest one that player is looking at and remove it.

	auto closestBlockRef = blocks.end();
	float closest_distance = MAX_RAY_DIST;

	for (auto it = blocks.begin(); it != blocks.end(); it++) {
		const Block& block = *it;
		if (glm::length(block.pos - camera.cameraPos) <= closest_distance && camera.camera_intersects_block(block)) {
			closest_distance = glm::length(block.pos - camera.cameraPos);
			closestBlockRef = it;
		}
	}

	if (closestBlockRef != blocks.end()) {
		isBlock[closestBlockRef->i][closestBlockRef->j][closestBlockRef->k] = false;
		blocks.erase(closestBlockRef);
	}
}

void Game::create_block() {
	// Find closest block similar to destroy_block
	// BUT, also find which face we are looking at and create new block on that face

	float closest_distance = MAX_RAY_DIST;
	auto closestBlockRef = blocks.end();

	for (auto it = blocks.begin(); it != blocks.end(); it++) {
		Block& block = *it;
		if (glm::length(block.pos - camera.cameraPos) <= closest_distance && camera.camera_intersects_block(block)) {
			closest_distance = glm::length(block.pos - camera.cameraPos);
			closestBlockRef = it;
		}
	}

	if (closestBlockRef == blocks.end()) {
		return;
	}

	glm::vec3 ac = camera.cameraPos - closestBlockRef->pos;
	float B = glm::dot(2 * ac, camera.cameraFront);
	float C = glm::length2(ac) - pow(BLOCK_RADIUS, 2);
	float D = pow(B, 2) - 4 * C;
	float lambda = 0.5f * (-B - std::sqrt(D)); // Eq of line is r = a + lambda * d, solving for lambda

	glm::vec3 intersecPoint = camera.cameraPos + camera.cameraFront * lambda;
	glm::vec3 localPoint = intersecPoint - closestBlockRef->pos; // transform to local coordinates of block
	glm::vec3 absPoint = glm::abs(localPoint);
	glm::vec3 hitNormal = glm::vec3(0.0f);
	int x = closestBlockRef->i;
	int y = closestBlockRef->j;
	int z = closestBlockRef->k;
	int dx = 0;
	int dy = 0;
	int dz = 0;
	if (absPoint.x > absPoint.y && absPoint.x > absPoint.z) { // if largest component is x, should build in x direction
		dx = (localPoint.x > 0.0f) ? 1.0f : -1.0f;
	}
	else if (absPoint.y > absPoint.x && absPoint.y > absPoint.z) {
		dy = (localPoint.y > 0.0f) ? 1.0f : -1.0f;
	}
	else {
		dz = (localPoint.z > 0.0f) ? 1.0f : -1.0f;
	}

	// block position + face normal vector * block size gives pos of new block
	if (0 <= x + dx && x + dx < WORLD_MAX_X && 0 <= y + dy && y + dy < WORLD_MAX_Y && 0 <= z + dz && z + dz < WORLD_MAX_Z) {
		blocks.push_back(Block(&worldShader, &VBOs, &VAOs, x + dx, y + dy, z + dz, blockToPlace));
		isBlock[x + dx][y + dy][z + dz] = true;
		if (collision_occurred(camera.cameraPos)) {
			blocks.pop_back();
			isBlock[x + dx][y + dy][z + dz] = false;
		}
		
	}	
}

void Game::gen_vbos_vaos() {
	for (unsigned int idx = 0; idx < numBlockTypes; idx++) {
		float vertices[] = {
			// Bottom face
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][0][0].first, texCoords[idx][0][0].second, // Bottom-left
			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][0][1].first, texCoords[idx][0][1].second, // Bottom-right
			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][0][3].first, texCoords[idx][0][3].second, // Top-right

			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][0][3].first, texCoords[idx][0][3].second, // Top-right
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][0][2].first, texCoords[idx][0][2].second, // Top-left
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][0][0].first, texCoords[idx][0][0].second, // Bottom-left

			// Top face
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][2][0].first, texCoords[idx][2][0].second, // Bottom-left
			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][2][1].first, texCoords[idx][2][1].second, // Bottom-right
			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][2][3].first, texCoords[idx][2][3].second, // Top-right

			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][2][3].first, texCoords[idx][2][3].second, // Top-right
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][2][2].first, texCoords[idx][2][2].second, // Top-left
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][2][0].first, texCoords[idx][2][0].second, // Bottom-left

			// Front face
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left
			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Bottom-right
			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right

			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Top-left
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left

			// Back face
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-left
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Top-left
			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right

			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-right
			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Bottom-right
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][0].first, texCoords[idx][1][0].second,  // Bottom-left

			// Left face
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-left
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right

			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right
			-BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-right
			-BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left

			// Right face
			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][2].first, texCoords[idx][1][2].second, // Bottom-left
			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][3].first, texCoords[idx][1][3].second, // Top-left
			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right

			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2, texCoords[idx][1][1].first, texCoords[idx][1][1].second, // Top-right
			 BLOCK_SIZE / 2, -BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][0].first, texCoords[idx][1][0].second, // Bottom-right
			 BLOCK_SIZE / 2,  BLOCK_SIZE / 2,  BLOCK_SIZE / 2, texCoords[idx][1][2].first, texCoords[idx][1][2].second  // Bottom-left
		};;
		glGenBuffers(1, &VBOs[idx]);
		glGenVertexArrays(1, &VAOs[idx]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[idx]);
		glBindVertexArray(VAOs[idx]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

bool Game::is_visible(const Block& block) const {
	// Check all the surrounding coordinates.
	int i = block.i;
	int j = block.j;
	int k = block.k;
	if (i == 0 || i == WORLD_MAX_X - 1 || j == 0 || j == WORLD_MAX_Y - 1 || k == 0 || k == WORLD_MAX_Z - 1) return true;
	if (!isBlock[i + 1][j][k] || !isBlock[i - 1][j][k] || !isBlock[i][j + 1][k] || !isBlock[i][j - 1][k] || !isBlock[i][j][k + 1] || !isBlock[i][j][k - 1]) return true;
	return false;
}

bool Game::collision_occurred(glm::vec3 pos) {
	// In order to collide, there must be overlap in x, y, AND z.
	for (const Block& block : blocks) {
		bool overlap_x = (pos.x - PLAYER_SIZE_X / 2 <= block.pos.x + BLOCK_SIZE / 2) && (pos.x + PLAYER_SIZE_X / 2 >= block.pos.x - BLOCK_SIZE / 2);
		bool overlap_y = (pos.y - PLAYER_SIZE_Y + CAMERA_Y_OFFSET <= block.pos.y + BLOCK_SIZE / 2) && (pos.y + CAMERA_Y_OFFSET >= block.pos.y - BLOCK_SIZE / 2);
		bool overlap_z = (pos.z - PLAYER_SIZE_Z / 2 <= block.pos.z + BLOCK_SIZE / 2) && (pos.z + PLAYER_SIZE_Z / 2 >= block.pos.z - BLOCK_SIZE / 2);
		if (overlap_x && overlap_y && overlap_z) {
			float player_bottom = pos.y - PLAYER_SIZE_Y + CAMERA_Y_OFFSET;
			float block_top = block.pos.y + BLOCK_SIZE / 2;

			if (player_bottom <= block_top && (player_bottom - block_top > -0.1f)) {
				playerOnGround = true;
			}

			return true;
		}
	}

	return false;
}