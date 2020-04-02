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
#include <map.h>
#include <player.h>

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
    ResourceManager Resources;
    Renderer Rendering;
    SpriteBatch Batcher;
    Shader *MapShader;
    std::unique_ptr<Map> GameMap;
    Camera cam;
    Player player;
};

void Initialize(Game *game, i32 width, i32 height)
{
    game->Batcher.init();
    game->Rendering = Renderer(width, height);
    game->cam = Camera(width, height);
}

void Load(Game *game)
{
    auto tile = LoadTexture(&game->Resources, "default.png");
    auto player = LoadTexture(&game->Resources, "rock.jpg");

    auto id = LoadShader(&game->Resources, "spriteBatchBorder");
    game->MapShader = &game->Resources.Shaders[LoadShader(&game->Resources, "map")];
    game->Batcher.default_shader = &game->Resources.Shaders[id];
    game->GameMap = LoadMap("./Data/Maps/Dungeon.tmx");
    Sprite s = {};
    s.Pos = Vector3(150, 100, 1);
    s.Dim = Vector3(20, 20, 1);
    s.Tex = game->Resources.Textures[player];
    game->player = {{0, 0}, s};
}

void Update(Game *game, f64 dt, Input const *mouse)
{
}

void FixedUpdate(Game *game, f64 dt, Input const *mouse)
{
    constexpr int PLAYER_COLLISION_PADDING = 12;
    auto oldPlayerPos = game->player.Graphic;
    game->player.Update(dt, *mouse);
    auto newPlayerPos = game->player.Graphic;
    for (const auto &layer : game->GameMap->Layers) {
        for (int i = 0; i < layer->m_tiles.size(); ++i) {
            auto const& tile = layer->m_tiles[i];
            auto PlayerRect = (Rectangle) {
                    newPlayerPos.Dim.x,
                    newPlayerPos.Dim.y,
                    newPlayerPos.Pos.x,
                    newPlayerPos.Pos.y
            };

            if ((tile.Flags != MapTileFlags::Walkable)
            && CheckCollisionRecs(PlayerRect, tile.Rect)) {
                // Reset player position (undo player position update!)
                game->player.Graphic.Pos = oldPlayerPos.Pos;
            }
        }
    }
    game->cam.Position = game->player.Graphic.Pos;
    game->cam.Update(dt, *mouse);
}

void Render(Game *game)
{
    game->Rendering.clear_screen(true, true);
    game->cam.Render();
    ShaderBind(game->MapShader);
    glUniform1i(glGetUniformLocation(game->MapShader->mProgram, "u_tileMap"), 0);
    glUniform1i(glGetUniformLocation(game->MapShader->mProgram, "u_lookupMap"), 1);
    glUniformMatrix4fv(glGetUniformLocation(game->MapShader->mProgram, "u_projectionMatrix"), 1, GL_FALSE, glm::value_ptr(game->cam.View));
    game->Rendering.push_render_state(sSpriteBatchState);
    // Draw floor
    game->GameMap->Layers[0]->draw();
    // Draw player
    ShaderBind(game->Batcher.default_shader);
    glUniformMatrix4fv(glGetUniformLocation(game->Batcher.default_shader->mProgram, "projection"), 1, GL_FALSE, glm::value_ptr(game->cam.View));
    game->Batcher.begin();
    game->Batcher.draw(game->player.Graphic);
    game->Batcher.end();
    game->Batcher.render_batches(&game->Rendering);
    // Draw walls
    ShaderBind(game->MapShader);
    game->GameMap->Layers[1]->draw();
    game->Rendering.pop_render_state();
}
