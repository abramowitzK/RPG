#pragma once
#include <vectorMath.h>

struct Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

static const Color RED = {255, 0, 0, 255};
struct Vertex2D
{
	Vector2 position;
	Color color;
	Vector2 uvs;
};
enum class GlyphSortType
{
	None,
	FrontToBack,
	BackToFront,
	Texture
};
