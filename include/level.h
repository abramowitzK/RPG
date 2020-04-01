#pragma once
#include <utils.h>
#include <player.h>
#include <comp/mapTile.h>
#include <tileSheet.h>

constexpr i32 Width = 20;
constexpr i32 Height = 20;
constexpr i32 NumTiles = Width * Height;
constexpr i32 TileWidth = 100;
constexpr i32 TileHeight = 100;
struct Level
{
    MapTile Tiles[NumTiles];
    Player PlayerModel;
    TileSheet Sheet;
};

// Demo function to fill level with all one type of tile;
Level LevelInit(Texture tex, Texture playerTex)
{
    Level level;
    // for (int i = 0; i < NumTiles; ++i)
    // {
    //     Sprite s = {};
    //     s.Dim = Vector3(TileWidth, TileHeight, 1);
    //     s.Tex = tex;
    //     int x = (i * TileWidth) % (Width * TileWidth);
    //     int y = ((i * TileHeight) / (Height * TileHeight)) * TileHeight;
    //     s.Pos = Vector3(x, y, 1);
    //     level.Tiles[i] = s;
    // }
    // Sprite s = {};
    // s.Dim = Vector3(TileWidth, TileHeight, 1);
    // s.Tex = playerTex;
    // level.PlayerModel = {{0, 0}, {}, s};
    // level.PlayerModel.Init(16, 20);
    return level;
}

void LevelUpdate(Level *level, double dt, Input const &input)
{
    level->PlayerModel.Update(dt, input);
    auto playerPos = level->PlayerModel.Position;
    if (playerPos.x < 50.0f)
    {
        level->PlayerModel.Position.x = 50.0f;
    }

    if (playerPos.x > (TileWidth * Width - 50))
    {
        level->PlayerModel.Position.x = TileWidth * Width - 50;
    }

    if (playerPos.y < 50.0f)
    {
        level->PlayerModel.Position.y = 50.0f;
    }

    if (playerPos.y > (TileHeight * Height - 50))
    {
        level->PlayerModel.Position.y = TileHeight * Height - 50;
    }
}
