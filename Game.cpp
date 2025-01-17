#include "Game.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Constants.h"
#include "BlockType.h"
#include "stb_image.h"

#include <unordered_map>
#include <vector>
#include <iostream>

Game::Game(GLFWwindow* window, glm::vec3 cameraStartPos) :
	VBOs(numBlockTypes, 0),
	VAOs(numBlockTypes, 0),
	camera(cameraStartPos),
	worldShader(WORLD_VERTEX_SHADER_PATH, WORLD_FRAGMENT_SHADER_PATH),
	crosshairShader(CROSSHAIR_VERTEX_SHADER_PATH, CROSSHAIR_FRAGMENT_SHADER_PATH),
	crosshair(&crosshairShader, CROSSHAIR_SIZE, CROSSHAIR_THICKNESS)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowUserPointer(window, this);

	blockToPlace = DIRT;
	lastFrame = 0.0f;
	lastClickEventTime = 0.0f;
	lastMousePosX = SCREEN_WIDTH / 2;
	lastMousePosY = SCREEN_HEIGHT / 2;
	isFirstMouse = true;

	blockPlaceKeyBinds = {
		{GLFW_KEY_1, SAND},
		{GLFW_KEY_2, GRASS},
		{GLFW_KEY_3, DIRT},
		{GLFW_KEY_4, GRAVEL},
		{GLFW_KEY_5, SNOW},
		{GLFW_KEY_6, CHERRY_LEAVES},
		{GLFW_KEY_7, OAK_LOG}
	};

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
		if (camera.block_in_frustum(block)) {
			block.draw();
		}
	}
	glDisable(GL_DEPTH_TEST); // To ensure crosshair is on top, turn off depth test
	crosshair.draw();
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Game::mouse_callback(GLFWwindow* window, double currMousePosX, double currMousePosY) {

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
}

void Game::scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	game->camera.FOV_Y -= SCROLL_SPEED * (float)y_offset;
	if (game->camera.FOV_Y < MIN_FOV_Y) {
		game->camera.FOV_Y = MIN_FOV_Y;
	}
	if (game->camera.FOV_Y > MAX_FOV_Y) {
		game->camera.FOV_Y = MAX_FOV_Y;
	}
	game->camera.FOV_X = game->camera.get_fov_x_deg(game->camera.FOV_Y);
}

void Game::process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	float currentTime = glfwGetTime();
	float deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	// Movement controls
	glm::vec3 planeUnitVector = camera.cameraFront - glm::dot(camera.cameraFront, camera.up) * camera.up;
	planeUnitVector = glm::normalize(planeUnitVector);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.cameraPos += planeUnitVector * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.cameraPos -= glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp)) * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.cameraPos -= planeUnitVector * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.cameraPos += camera.cameraRight * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.cameraPos += camera.up * PLAYER_SPEED * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.cameraPos -= camera.up * PLAYER_SPEED * deltaTime;
	}
	
	if (glfwGetTime() - lastClickEventTime >= CLICK_COOLDOWN_TIME && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		lastClickEventTime = glfwGetTime();
		destroy_block();
	}
	if (glfwGetTime() - lastClickEventTime >= CLICK_COOLDOWN_TIME && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		lastClickEventTime = glfwGetTime();
		create_block();
	}

	for (auto it = blockPlaceKeyBinds.begin(); it != blockPlaceKeyBinds.end(); it++) {
		if (glfwGetKey(window, it->first) == GLFW_PRESS) {
			blockToPlace = it->second;
		}
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

int Game::get_terrain_height(int x, int z, int maxHeight) {
	return (int)(maxHeight * (1.1 + cos((float)x/15.0f) * cos((float)z/15.0f)));
}

void Game::generate_terrain() {
	int maxHeight = 20;
	for (unsigned int i = 0; i < 200; i++) {
		for (unsigned int k = 0; k < 200; k++) {
			int height = get_terrain_height(i, k, maxHeight);
			for (int j = 0; j < height - 1; j++) {
				Block block(&worldShader, &VBOs, &VAOs, glm::vec3(i * BLOCK_SIZE, j * BLOCK_SIZE, k * BLOCK_SIZE), DIRT);
				blocks.push_back(block);
			}
			Block block(&worldShader, &VBOs, &VAOs, glm::vec3(i * BLOCK_SIZE, (height - 1) * BLOCK_SIZE, k * BLOCK_SIZE), GRASS);
			blocks.push_back(block);
		}
	}
}

void Game::destroy_block() {
	// Iterate through all blocks to find closest one that player is looking at and remove it.

	float closest_distance = MAX_RAY_DIST;
	auto closestBlockRef = blocks.end();

	for (auto it = blocks.begin(); it != blocks.end(); it++) {
		Block& block = *it;
		if (glm::length(block.pos - camera.cameraPos) <= closest_distance && camera.camera_intersects_block(block)) {
			closest_distance = glm::length(block.pos - camera.cameraPos);
			closestBlockRef = it;
		}
	}

	if (closestBlockRef != blocks.end()) {
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
	if (absPoint.x > absPoint.y && absPoint.x > absPoint.z) { // if largest component is x, should build in x direction
		hitNormal = glm::vec3((localPoint.x > 0) ? 1.0f : -1.0f, 0.0f, 0.0f); // positive means positive normal
	}
	else if (absPoint.y > absPoint.x && absPoint.y > absPoint.z) {
		hitNormal = glm::vec3(0.0f, (localPoint.y > 0) ? 1.0f : -1.0f, 0.0f);
	}
	else {
		hitNormal = glm::vec3(0.0f, 0.0f, (localPoint.z > 0) ? 1.0f : -1.0f);
	}

	// block position + face normal vector * block size gives pos of new block
	blocks.push_back(Block(&worldShader, &VBOs, &VAOs, closestBlockRef->pos + hitNormal*BLOCK_SIZE, blockToPlace));
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
