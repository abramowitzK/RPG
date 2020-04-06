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
#include <imgui.h>

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
    MapTile selected;
    Vector2 converted;
    Path PlayerPath;
};

void Initialize(Game *game, i32 width, i32 height)
{
    game->Batcher.init();
    game->Rendering = Renderer(width, height);
    game->cam = Camera(width, height);
}

void Load(Game *game)
{
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

void Update(Game *game, f64 dt, Input const *input)
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

    if (input->Mouse.GetLeftButtonDownThisFrame())
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
}

void FixedUpdate(Game *game, f64 dt, Input const *mouse)
{
    auto oldPlayerPos = game->player.Graphic.Pos;
    game->player.Update(dt, *mouse);
    auto deltas = glm::abs(oldPlayerPos - game->player.Graphic.Pos);
    // for (const auto &layer : game->GameMap->Layers)
    // {
    //     for (int i = 0; i < layer->m_tiles.size(); ++i)
    //     {
    //         auto const &tile = layer->m_tiles[i];
    //         Rectangle PlayerRect = {
    //             game->player.Graphic.Dim.x,
    //             game->player.Graphic.Dim.y,
    //             game->player.Graphic.Pos.x,
    //             game->player.Graphic.Pos.y};

    //         if (tile.Flags != MapTileFlags::Walkable)
    //         {
    //             const float Distance = 26.0f;
    //             auto playerCenter = PlayerRect.GetCenter();
    //             auto tileCenter = tile.Rect.GetCenter();
    //             auto distVector = playerCenter - tileCenter;
    //             float xDepth = Distance - std::abs(distVector.x);
    //             float yDepth = Distance - std::abs(distVector.y);
    //             // If both the depths are > 0, then we collided
    //             if (xDepth > 0 && yDepth > 0)
    //             {
    //                 // Check which collision depth is less
    //                 if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f))
    //                 {
    //                     // X collision depth is smaller so we push in X direction
    //                     if (distVector.x < 0)
    //                     {
    //                         game->player.Graphic.Pos.x -= xDepth;
    //                     }
    //                     else
    //                     {
    //                         game->player.Graphic.Pos.x += xDepth;
    //                     }
    //                 }
    //                 else
    //                 {
    //                     // Y collision depth is smaller so we push in X direction
    //                     if (distVector.y < 0)
    //                     {
    //                         game->player.Graphic.Pos.y -= yDepth;
    //                     }
    //                     else
    //                     {
    //                         game->player.Graphic.Pos.y += yDepth;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }

    game->cam.Position = game->player.Graphic.Pos;
    game->cam.Update(dt, *mouse);
}

void Render(Game *game)
{

    static bool RenderFloor;
    static bool RenderWalls;
    ImGui::Begin("Debug Options", 0);
    ImGui::Checkbox("RenderFloor", &RenderFloor);
    ImGui::Checkbox("RenderWalls", &RenderWalls);
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
    game->Batcher.end();
    game->Batcher.render_batches(&game->Rendering);
    // Draw walls
    ShaderBind(game->MapShader);
    if (RenderWalls)
    {
        game->GameMap->Layers[1]->draw();
    }
    game->Rendering.pop_render_state();

    ImGui::Begin("Player Pos", 0);
    ImGui::Text("%f", game->player.Graphic.Pos.x);
    ImGui::Text("%f", game->player.Graphic.Pos.y);
    ImGui::End();

    ImGui::Begin("Selected", 0);
    ImGui::Text("%f", game->selected.Rect.x);
    ImGui::Text("%f", game->selected.Rect.y);
    ImGui::End();

    ImGui::Begin("Converted", 0);
    ImGui::Text("%f", game->converted.x);
    ImGui::Text("%f", game->converted.y);
    ImGui::End();
}
