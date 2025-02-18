#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

class Game;

class UIManager {
public:
	GLFWwindow* window;
	ImFont* regularFont;
	Game* game;
	const float alpha;
	float avg_fps;
	UIManager(GLFWwindow* window, Game* game);
	~UIManager();
	void render_overlay();
	void render_pause_screen();
	void trans_to_game();
	void trans_to_settings();
	void draw();
	void update_avg_fps();
};