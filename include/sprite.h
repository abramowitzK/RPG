#pragma once
#include <texture.h>
#include <vectorMath.h>
#include <structures.h>

struct Sprite
{
	Texture Tex;
	Vector3 Pos;
	Vector3 Dim;
	Color Col;
	float depth;
	float angle;
};
