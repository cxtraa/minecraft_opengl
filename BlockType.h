#pragma once

/*
* All block types in game.
*/

#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum BlockType {
	NONE,
	SAND,
	GRASS,
	DIRT,
	GRAVEL,
	SNOW,
	CHERRY_LEAVES,
	OAK_LOG
};

const std::unordered_map<BlockType, int> blockToIdx = {
	{OAK_LOG, 0},
	{CHERRY_LEAVES, 1},
	{SNOW, 2},
	{GRAVEL, 3},
	{DIRT, 4},
	{GRASS, 5},
	{SAND, 6},
	{NONE, 7}
};

const std::unordered_map<int, BlockType> keyToBlock = {
	{GLFW_KEY_1, SAND},
	{GLFW_KEY_2, GRASS},
	{GLFW_KEY_3, DIRT},
	{GLFW_KEY_4, GRAVEL},
	{GLFW_KEY_5, SNOW},
	{GLFW_KEY_6, CHERRY_LEAVES},
	{GLFW_KEY_7, OAK_LOG}
};