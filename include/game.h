#pragma once
#include <vectorMath.h>
#include <game.h>
#include <input.h>
#include <glew.h>

enum GameState
{
    Menu,
    SettingsMenu,
    Playing,
    Quit
};

enum Intents
{
    Nothing,
    Up,
    Down
};

struct Game
{
    GameState State;
};

void Initialize(Game *game, i32 height, i32 width)
{
}

void Update(Game *game, f64 dt, Input const *mouse)
{
}

void FixedUpdate(Game *game, f64 dt, Input const *mouse)
{
}

void Render(Game *game)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
