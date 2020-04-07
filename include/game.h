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
#include <map.h>
#include <player.h>
#include <imgui.h>
#include <enemies.h>

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
    static const int NumEnemies = 16;
    GameState State;
    ResourceManager Resources;
    Renderer Rendering;
    SpriteBatch Batcher;
    Shader *MapShader;
    std::unique_ptr<Map> GameMap;
    Camera cam;
    Player player;
    MapTile selected;
    Vector2 converted;
    Path PlayerPath;
    enemy_t Enemies[NumEnemies];
    i32 CurrentEnemyIndex = 0;
    Sprite EnemySprite;
};

void Initialize(Game *game, i32 width, i32 height)
{
    game->Batcher.init();
    game->Rendering = Renderer(width, height);
    game->cam = Camera(width, height);
    memset(game->Enemies, 0, ARRAY_SIZE(game->Enemies) * sizeof(enemy_t));
}

void Load(Game *game)
{
    auto player = LoadTexture(&game->Resources, "rock.jpg");
    auto id = LoadShader(&game->Resources, "spriteBatchBorder");
    auto enemy = LoadTexture(&game->Resources, "kyle.jpg");
    game->MapShader = &game->Resources.Shaders[LoadShader(&game->Resources, "map")];
    game->Batcher.default_shader = &game->Resources.Shaders[id];
    game->GameMap = LoadMap("./Data/Maps/Dungeon.tmx");
    Sprite s = {};
    s.Pos = Vector3(150, 100, 1);
    s.Dim = Vector3(20, 20, 1);
    s.Tex = game->Resources.Textures[player];
    game->player = {{0, 0}, s};

    Sprite enemySprite = {};
    enemySprite.Pos = Vector3(150, 100, 1);
    enemySprite.Dim = Vector3(20, 20, 1);
    enemySprite.Tex = game->Resources.Textures[player];
    game->EnemySprite = enemySprite;
}

void Update(Game *game, f64 dt, Input const *input, f64 ticks)
{
    if (input->Intents == Intent::Resize)
    {
        game->cam.Resize(input->x, input->y);
    }

    auto converted = game->cam.ConvertScreenToWorld({input->Mouse.GetMouseX(), input->Mouse.GetMouseY()});
    if (input->Mouse.GetLeftButtonDown())
    {
        for (const auto &layer : game->GameMap->Layers)
        {
            for (const auto &tile : layer->m_tiles)
            {
                if (IsPointInRect(tile.Rect, converted))
                {
                    game->selected = tile;
                }
            }
        }
    }

    if (input->Mouse.GetLeftButtonHeld())
    {
        game->player.Path = {};
        game->player.CurrentIndexInPath = 1;
        if (game->GameMap->Layers[1]->FindPath(game->player.Graphic.Pos, converted, &game->player.Path))
        {
        }
        game->GameMap->Layers[0]->RestoreOldPixelData();
        for (int i = 0; i <= game->player.Path.Index; ++i)
        {
            game->GameMap->Layers[0]->HighlightTile(game->player.Path.Tiles[i].Index * 2);
        }
    }

    for (i32 i = 0; i < game->CurrentEnemyIndex; ++i)
    {
        if (ticks - game->Enemies[i].LastTickTime > EnemyTickTime)
        {
            game->Enemies[i].CurrentPath = {};
            game->Enemies[i].CurrentIndexInPath = 1;
            if (game->GameMap->Layers[1]->FindPath(game->Enemies[i].Graphic.Pos, game->player.Graphic.Pos, &game->Enemies[i].CurrentPath))
            {
            }
            game->Enemies[i].LastTickTime = ticks;
        }
    }
}

void FixedUpdate(Game *game, f64 dt, Input const *mouse, f64 ticks)
{
    game->player.Update(dt, *mouse);
    game->cam.Position = game->player.Graphic.Pos;
    game->cam.Update(dt, *mouse);
    for (i32 i = 0; i < game->CurrentEnemyIndex; ++i)
    {
        game->Enemies[i].Update(dt);
    }
}

void Render(Game *game)
{

    static bool RenderFloor = true;
    static bool RenderWalls = true;
    ImGui::Begin("Debug Options", 0);
    ImGui::Checkbox("RenderFloor", &RenderFloor);
    ImGui::Checkbox("RenderWalls", &RenderWalls);
    ImGui::Text("PathLength %d", game->player.Path.Index + 1);
    ImGui::Text("Num Enemies %d", game->CurrentEnemyIndex + 1);
    if (ImGui::Button("Spawn Enemy"))
    {
        if (game->CurrentEnemyIndex < Game::NumEnemies)
        {
            game->Enemies[game->CurrentEnemyIndex++] = {game->EnemySprite};
        }
    }
    ImGui::End();
    game->Rendering.clear_screen(true, true);
    game->cam.Render();
    ShaderBind(game->MapShader);
    glUniform1i(glGetUniformLocation(game->MapShader->mProgram, "u_tileMap"), 0);
    glUniform1i(glGetUniformLocation(game->MapShader->mProgram, "u_lookupMap"), 1);
    glUniformMatrix4fv(glGetUniformLocation(game->MapShader->mProgram, "u_projectionMatrix"), 1, GL_FALSE, glm::value_ptr(game->cam.View));
    game->Rendering.push_render_state(sSpriteBatchState);
    // Draw floor
    if (RenderFloor)
    {
        game->GameMap->Layers[0]->draw();
    }
    // Draw player
    ShaderBind(game->Batcher.default_shader);
    glUniformMatrix4fv(glGetUniformLocation(game->Batcher.default_shader->mProgram, "projection"), 1, GL_FALSE, glm::value_ptr(game->cam.View));
    game->Batcher.begin();
    game->Batcher.draw(game->player.Graphic);
    for (int i = 0; i < game->CurrentEnemyIndex; i++)
    {
        game->Batcher.draw(game->Enemies[i].Graphic);
    }
    game->Batcher.end();
    game->Batcher.render_batches(&game->Rendering);
    // Draw walls
    ShaderBind(game->MapShader);
    if (RenderWalls)
    {
        game->GameMap->Layers[1]->draw();
    }
    game->Rendering.pop_render_state();
}
