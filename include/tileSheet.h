#pragma once
#include <texture.h>
#include <vectorMath.h>

struct TileSheet
{
    Texture Tex;
    int Width;  // Number of horizontal sprites
    int Height; // Number of vertical sprites
};

Vector4 GetFrameUvs(TileSheet *tileSheet, int index)
{
    int xTile = index % tileSheet->Width;
    int yTile = index / tileSheet->Width;

    Vector4 uvs;
    uvs.x = xTile / (float)tileSheet->Width;
    uvs.y = yTile / (float)tileSheet->Height;
    uvs.z = 1.0f / tileSheet->Width;
    uvs.w = 1.0f / tileSheet->Height;

    return uvs;
}