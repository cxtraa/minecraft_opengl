#pragma once

#include <unordered_map>
#include <cmath>
#include <glm/glm.hpp>

#include "BlockType.h"

// Screen constants
const float SCREEN_WIDTH = 1920.0f;
const float SCREEN_HEIGHT = 1080.0f;
const float ASPECT_RATIO = SCREEN_WIDTH / SCREEN_HEIGHT;

// Movement / mouse control
const float SENSITIVITY = 0.1f;

// Various filepaths
const char* const WORLD_VERTEX_SHADER_PATH = "shader.vert";
const char* const WORLD_FRAGMENT_SHADER_PATH = "shader.frag";
const char* const CROSSHAIR_VERTEX_SHADER_PATH = "ui_shader.vert";
const char* const CROSSHAIR_FRAGMENT_SHADER_PATH = "ui_shader.frag";
const char* const TEXTURE_PATH = "textures/tex_array_0.png";

// Texture dimensions
const int numBlockTypes = 8;
const int numTexturesX = 3;
const int numTexturesY = 8;

const float texW = 1.0f / (float)numTexturesX; // Width of a single texture in atlas.
const float texH = 1.0f / (float)numTexturesY;
const float halfPixel = 0.0f;

// Block dimensions
const float BLOCK_SIZE = 0.5f;
const float BLOCK_RADIUS = BLOCK_SIZE * std::sqrt(3) * 0.5f; // We use a sphere bounding box for block for simplicity.
const float EPSILON = 0.0f;

const float PLAYER_SIZE_X = 0.4f;
const float PLAYER_SIZE_Y = 0.9f;
const float PLAYER_SIZE_Z = 0.4f;
const float CAMERA_Y_OFFSET = 0.2f;

// Physics
const float WORLD_GRAVITY = -9.8f;
const float JUMP_SPEED = std::sqrt(2.0f * std::abs(WORLD_GRAVITY) * BLOCK_SIZE) + 0.2f;
const float PLAYER_SPEED = 2.5f;

// FOV settings
const float DEFAULT_FOV_Y = 60.0f;
const float MIN_FOV_Y = 1.0f;
const float MAX_FOV_Y = 120.0f;
const float SCROLL_SPEED = 5.0f; // for changing FOV

// Render distance settings
const float NEAR = 0.1f; // closest distance to render
const float FAR = 25.0f; // furthest dist to render

// Block interaction settings
const float MAX_RAY_DIST = 6.0f * BLOCK_SIZE;
const float CLICK_COOLDOWN_TIME = 0.2f;

// Crosshair geometry
const float CROSSHAIR_THICKNESS = 3.0f;
const float CROSSHAIR_SIZE = 25.0f;

// World dimensions
const int WORLD_MAX_Z = 100;
const int WORLD_MAX_Y = 40;
const int WORLD_MAX_X = 100;