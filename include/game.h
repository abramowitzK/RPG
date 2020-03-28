#pragma once
#include <vectorMath.h>
#include <game.h>
#include <input.h>
#include <glew.h>
#include <resourceLoader.h>
#include <rendering.h>
#include <spriteBatch.h>
#include <camera.h>
#include <glm/gtc/type_ptr.hpp>
#include <level.h>

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
Level GameLevel;

struct Game
{
    GameState State;
    ResourceManager Resources;
    Renderer Rendering;
    SpriteBatch Batcher;
};

void Initialize(Game *game, i32 width, i32 height)
{
    game->Batcher.init();
    game->Rendering = Renderer(width, height);
}

void Load(Game *game)
{
    auto tile = LoadTexture(&game->Resources, "default.png");
    auto player = LoadTexture(&game->Resources, "rock.jpg");

    auto id = LoadShader(&game->Resources, "spriteBatchBorder");
    game->Batcher.default_shader = &game->Resources.Shaders[id];
    GameLevel = LevelInit(game->Resources.Textures[tile], game->Resources.Textures[player]);
}

void Update(Game *game, f64 dt, Input const *mouse)
{
    LevelUpdate(&GameLevel, dt, *mouse);
}

void FixedUpdate(Game *game, f64 dt, Input const *mouse)
{
}

void Render(Game *game)
{
    game->Rendering.clear_screen(true, true);
    GameLevel.Camera.Render();
    auto projection = GameLevel.Camera.View;
    ShaderBind(game->Batcher.default_shader);
    auto loc = glGetUniformLocation(game->Batcher.default_shader->mProgram, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
    game->Batcher.begin();
    for (int i = 0; i < NumTiles; ++i)
    {
        game->Batcher.draw(GameLevel.Tiles[i]);
    }
    game->Batcher.draw(GameLevel.PlayerModel.Graphic);
    game->Batcher.end();
    game->Batcher.render_batches(&game->Rendering);
}
