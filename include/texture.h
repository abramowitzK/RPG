#pragma once
#include <glew.h>
#include <string>
struct Texture
{
	GLuint Tex;
	int width;
	int height;
	int comp;
	char Name[64];
};

Texture create_2d_texture(const char *filepath);