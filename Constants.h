#pragma once

#include <unordered_map>

#include "BlockType.h"

// Screen dimensions
const float screenWidth = 1920;
const float screenHeight = 1080;

const float sensitivity = 0.1f;
const float PLAYER_SPEED = 2.5f;

const char* const VERTEX_SHADER_PATH = "shader.vert";
const char* const FRAGMENT_SHADER_PATH = "shader.frag";
const char* const TEXTURE_PATH = "textures/tex_array_0.png";

const int numTexturesX = 3;
const int numTexturesY = 8;

const float blockSize = 0.5f;

const float texW = 1.0f / (float)numTexturesX;
const float texH = 1.0f / (float)numTexturesY;

const float halfPixel = 1.0f/64.0f;
