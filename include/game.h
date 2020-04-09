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
#include <bullet.h>

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
    static const int NumBullets = 16;
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
    bullet_t Bullets[NumBullets];
    i32 CurrentBulletIndex = 0;
    Sprite BulletSprite;
    f64 LastPlayerBullet = 0.0;
};

void Initialize(Game *game, i32 width, i32 height)
{
    game->Batcher.init();
    game->Rendering = Renderer(width, height);
    game->cam = Camera(width, height);
    memset(game->Enemies, 0, ARRAY_SIZE(game->Enemies) * sizeof(enemy_t));
    memset(game->Bullets, 0, ARRAY_SIZE(game->Bullets) * sizeof(bullet_t));
}

void Load(Game *game)
{
    auto player = LoadTexture(&game->Resources, "rock.jpg");
    auto id = LoadShader(&game->Resources, "spriteBatchBorder");
    auto bullet = LoadTexture(&game->Resources, "default.png");
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

    Sprite bulletSprite = {};
    bulletSprite.Dim = Vector3(5, 5, 1);
    bulletSprite.Tex = game->Resources.Textures[bullet];
    game->BulletSprite = bulletSprite;
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

    if (input->Keys.IsKeyPressed(Keys::Space)) {
        auto dir = glm::normalize(converted - Vector2(game->player.Graphic.Pos));
        if(ticks - game->LastPlayerBullet > 0.25) {
            game->LastPlayerBullet = ticks;
            if (game->CurrentBulletIndex < game->NumBullets - 1) {
                Sprite b = game->BulletSprite;
                b.Pos = game->player.Graphic.Pos + 10.0f;
                game->Bullets[game->CurrentBulletIndex++] = {ticks, b, dir};
            }
        }
    }

    {
        i32 NumBulletsDeleted = 0;
        i32 KnownGoodIndex = 0;
        for (int i = 0; i < game->CurrentBulletIndex; i++) {
            if ((ticks - game->Bullets[i].StartTime) < BulletTimeToLive) {
                game->Bullets[KnownGoodIndex++] = game->Bullets[i];
            } else {
                NumBulletsDeleted++;
            }
        }

        game->CurrentBulletIndex -= NumBulletsDeleted;
    }

    {
        i32 NumEnemiesDeleted = 0;
        i32 KnownGoodIndex = 0;
        for (int i = 0; i < game->CurrentEnemyIndex; i++) {
            if (game->Enemies[i].Health > 0) {
                game->Enemies[KnownGoodIndex++] = game->Enemies[i];
            } else {
                NumEnemiesDeleted++;
            }
        }

        game->CurrentEnemyIndex -= NumEnemiesDeleted;
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

    for (i32 i = 0; i < game->CurrentBulletIndex; ++i)
    {
        game->Bullets[i].Graphic.Pos += Vector3(BulletSpeed * game->Bullets[i].Direction, 1.0);
        for (i32 j = 0; j < game->CurrentEnemyIndex; j++) {
            Rectangle rect = {};
            enemy_t e =  game->Enemies[j];
            rect.x = e.Graphic.Pos.x;
            rect.y = e.Graphic.Pos.y;
            rect.height = 20;
            rect.width = 20;
            if (IsPointInRect(rect, game->Bullets[i].Graphic.Pos)) {
                game->Enemies[j].Health = 0;
                game->Bullets[i].StartTime = 0; // mark to be destroyed
            }
        }
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
    ImGui::Text("Num Enemies %d", game->CurrentEnemyIndex);
    ImGui::Text("Num Bullets %d", game->CurrentBulletIndex);
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

    for (int i = 0; i < game->CurrentBulletIndex; i++)
    {
        game->Batcher.draw(game->Bullets[i].Graphic);
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
