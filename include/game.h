#pragma once
#include <SDL.h>
#include <ttf.h>
#include <vector_math.h>
#include <game.h>
#include <menu.h>
#include <input.h>

constexpr i32 PaddleHeight = 100;
constexpr i32 PaddleWidth = 20;
constexpr i32 PaddleHeightOver2 = PaddleHeight / 2;
constexpr i32 BallWidth = 20;
constexpr f32 BallSpeed = 150.0f;
constexpr i32 PlayerSpeed = 100.0f;
constexpr f32 AISpeed = 115.0f;
constexpr i32 ScoreToWin = 5;

enum GameState
{
    Menu,
    SettingsMenu,
    Playing,
    ScoredLeft,
    ScoredRight,
    LeftWon,
    RightWon,
    Quit
};

enum Intents
{
    Nothing,
    Up,
    Down
};

struct Ball
{
    SDL_Rect *Rect;
    v2 Vel;
    v2 Pos;
};

struct Paddle
{
    SDL_Rect *Rect;
    f32 Pos;
    f32 Vel;
    i32 Score;
};

struct Game
{
    GameMenu GameMenu;
    GameState State;
    SDL_Rect GameObjects[6];
    Ball GameBall;
    Paddle LeftPaddle;
    Paddle RightPaddle;
    Intents CurrentIntent;
    SDL_Renderer *Renderer;
    TTF_Font *Font;
    SDL_Window *Window;
    RenderableText LeftScore;
    RenderableText RightScore;
    RenderableText LeftHasScoredServingTowardsLeft;
    RenderableText RightHasScoredServingTowardsRight;
    RenderableText LeftWon;
    RenderableText RightWon;
    i32 Width;
    i32 Height;
};

void UpdateScoreText(Game *game, SDL_Renderer *renderer, TTF_Font *font)
{
    char left[64] = {};
    SDL_itoa(game->LeftPaddle.Score, left, 10);
    char right[64] = {};
    SDL_itoa(game->RightPaddle.Score, right, 10);
    // Free the textures
    if (game->LeftScore.Texture)
    {
        SDL_DestroyTexture(game->LeftScore.Texture);
    }

    if (game->RightScore.Texture)
    {
        SDL_DestroyTexture(game->RightScore.Texture);
    }

    game->LeftScore = GetTextureFromString(renderer, left, {0, 0, 255, 255}, font);
    game->RightScore = GetTextureFromString(renderer, right, {255, 0, 0, 255}, font);
}

i32 Initialize(Game *game, i32 height, i32 width)
{
    game->State = GameState::Menu;
    game->GameBall = {&game->GameObjects[0], {BallSpeed, 0}, {width / 2.0f - BallWidth / 2.0f, height / 2.0f - BallWidth / 2.0f}};
    game->LeftPaddle = {&game->GameObjects[1], height / 2.0f - PaddleHeightOver2, 0.0f, 0};
    game->RightPaddle = {&game->GameObjects[2], height / 2.0f - PaddleHeightOver2, 0.0f, 0};
    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        return -1;
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    if (0 != SDL_CreateWindowAndRenderer(width, height, 0, &game->Window, &game->Renderer))
    {
        return -2;
    }

    if (0 != TTF_Init())
    {
        return -3;
    }

    game->Font = TTF_OpenFont("fonts/arial.ttf", 20);
    // Left paddle
    game->GameObjects[1] = {0, (int)height / 2 - PaddleHeightOver2, PaddleWidth, PaddleHeight};
    // Right paddle
    game->GameObjects[2] = {(int)width - PaddleWidth, (int)height / 2 - PaddleHeightOver2, PaddleWidth, PaddleHeight};
    // Ball
    game->GameObjects[0] = {(int)width / 2, (int)height / 2, BallWidth, BallWidth};
    // Top rebounder
    game->GameObjects[3] = {0, -20, (int)width, 20};
    // Bottom rebounder
    game->GameObjects[4] = {0, (int)height, (int)width, 20};
    game->Width = width;
    game->Height = height;
    UpdateScoreText(game, game->Renderer, game->Font);
    game->GameMenu = MenuInitialize(width, height);
    MenuAddOption(&game->GameMenu, game->Renderer, "Play Game", {255, 255, 255, 255});
    MenuAddOption(&game->GameMenu, game->Renderer, "Settings", {255, 255, 255, 255});
    MenuAddOption(&game->GameMenu, game->Renderer, "Exit", {255, 255, 255, 255});
    game->LeftHasScoredServingTowardsLeft = GetTextureFromString(game->Renderer, "Point for Left Side! Service towards Right. Press Spacebar to serve", {255, 255, 255, 255}, game->Font);
    game->RightHasScoredServingTowardsRight = GetTextureFromString(game->Renderer, "Point for Right Side! Service towards Right. Press Spacebar to serve", {255, 255, 255, 255}, game->Font);
    game->LeftWon = GetTextureFromString(game->Renderer, "Left Won! Press spacebar to play again", {255, 255, 255, 255}, game->Font);
    game->RightWon = GetTextureFromString(game->Renderer, "Right Won! Press spacebar to play again", {255, 255, 255, 255}, game->Font);
    return 0;
}

void Update(Game *game, f64 dt, KeyState *keys, MouseState *mouse)
{
    if (game->LeftPaddle.Score >= ScoreToWin)
    {
        // Left won
        game->State = GameState::LeftWon;
    }
    else if (game->RightPaddle.Score >= ScoreToWin)
    {
        game->State = GameState::RightWon;
    }

    switch (game->State)
    {
    case GameState::Menu:
    {
        if (KeyPressedThisFrame(keys, SDL_SCANCODE_DOWN))
        {
            game->GameMenu.SelectedMenuOption = (game->GameMenu.SelectedMenuOption + 1) % game->GameMenu.NumMenuOptions;
        }
        else if (KeyPressedThisFrame(keys, SDL_SCANCODE_UP))
        {
            game->GameMenu.SelectedMenuOption--;
            if (game->GameMenu.SelectedMenuOption < 0)
            {
                game->GameMenu.SelectedMenuOption = game->GameMenu.NumMenuOptions - 1;
            }
        }
        else if (KeyPressedThisFrame(keys, SDL_SCANCODE_SPACE))
        {
            // Perform action for currently selected menu item
            MenuItems item = (MenuItems)game->GameMenu.SelectedMenuOption;
            switch (item)
            {
            case MenuItemsPlay:
            {
                game->State = GameState::Playing;
            }
            break;
            case MenuItemsQuit:
            {
                game->State = GameState::Quit;
            }
            break;
            }
        }

        // TODO mouse selection
        MenuUpdate(&game->GameMenu, dt);
    }
    break;
    case GameState::Playing:
    {
        if (KeyPressedThisFrame(keys, SDL_SCANCODE_ESCAPE))
        {
            game->State = GameState::Menu;
        }
    }
    break;
    case GameState::ScoredRight:
    case GameState::ScoredLeft:
    {
        if (KeyPressedThisFrame(keys, SDL_SCANCODE_SPACE))
        {
            game->State = GameState::Playing;
        }
    }
    break;
    case GameState::LeftWon:
    case GameState::RightWon:
    {
        if (KeyPressedThisFrame(keys, SDL_SCANCODE_SPACE))
        {
            game->GameBall = {&game->GameObjects[0], {BallSpeed, 0}, {game->Width / 2.0f - BallWidth / 2.0f, game->Height / 2.0f - BallWidth / 2.0f}};
            game->LeftPaddle = {&game->GameObjects[1], game->Height / 2.0f - PaddleHeightOver2, 0.0f, 0};
            game->RightPaddle = {&game->GameObjects[2], game->Height / 2.0f - PaddleHeightOver2, 0.0f, 0};
            game->State = GameState::Menu;
            UpdateScoreText(game, game->Renderer, game->Font);
        }
    }
    break;
    }
}

void FixedUpdate(Game *game, f64 dt, KeyState *keys, MouseState *mouse)
{
    if (GameState::Menu != game->State)
    {
        if (IsKeyPressed(keys, SDL_SCANCODE_DOWN))
        {
            game->LeftPaddle.Vel = PlayerSpeed;
        }
        else if (IsKeyPressed(keys, SDL_SCANCODE_UP))
        {
            game->LeftPaddle.Vel = -PlayerSpeed;
        }
        else
        {
            game->LeftPaddle.Vel = 0.0f;
        }

        game->LeftPaddle.Pos += (f32)((f64)game->LeftPaddle.Vel * dt * 0.5);
        game->LeftPaddle.Rect->y = (i32)game->LeftPaddle.Pos;
        game->RightPaddle.Pos += (f32)((f64)game->RightPaddle.Vel * dt * 0.5);
        game->RightPaddle.Rect->y = (i32)game->RightPaddle.Pos;
        if (game->LeftPaddle.Pos < 0)
        {
            game->LeftPaddle.Pos = 0;
        }

        if (game->LeftPaddle.Pos > game->Height - game->LeftPaddle.Rect->h)
        {
            game->LeftPaddle.Pos = (f32)(game->Height - game->LeftPaddle.Rect->h);
        }

        if (game->RightPaddle.Pos < 0)
        {
            game->RightPaddle.Pos = 0;
        }

        if (game->RightPaddle.Pos > game->Height - game->RightPaddle.Rect->h)
        {
            game->RightPaddle.Pos = (f32)(game->Height - game->RightPaddle.Rect->h);
        }
        if (GameState::Playing == game->State)
        {

            game->GameBall.Pos.x += (f32)((f64)game->GameBall.Vel.x * dt * 0.5);
            game->GameBall.Pos.y += (f32)((f64)game->GameBall.Vel.y * dt * 0.5);
            game->GameBall.Rect->x = (i32)game->GameBall.Pos.x;
            game->GameBall.Rect->y = (i32)game->GameBall.Pos.y;
            if (SDL_HasIntersection(game->GameBall.Rect, game->LeftPaddle.Rect))
            {
                // Calculate distance from center of ball to center of paddle (pos or neg)
                f32 relativeIntersect = (game->LeftPaddle.Pos + PaddleHeightOver2) - (game->GameBall.Pos.y + BallWidth / 2.0f);
                f32 normalizedRelativeIntersect = (relativeIntersect / PaddleHeightOver2);
                f32 bounceAngle = normalizedRelativeIntersect * ((float)M_PI / 4.0f);
                game->GameBall.Vel.x = BallSpeed * cosf(bounceAngle);
                game->GameBall.Vel.y = BallSpeed * -sinf(bounceAngle);
            }

            if (SDL_HasIntersection(game->GameBall.Rect, game->RightPaddle.Rect))
            {
                f32 relativeIntersect = (game->RightPaddle.Pos + PaddleHeightOver2) - (game->GameBall.Pos.y + BallWidth / 2.0f);
                f32 normalizedRelativeIntersect = (relativeIntersect / PaddleHeightOver2);
                f32 bounceAngle = normalizedRelativeIntersect * ((float)M_PI / 4.0f);
                game->GameBall.Vel.x = BallSpeed * -cosf(bounceAngle);
                game->GameBall.Vel.y = BallSpeed * -sinf(bounceAngle);
            }

            if (SDL_HasIntersection(game->GameBall.Rect, &game->GameObjects[3]) || SDL_HasIntersection(game->GameBall.Rect, &game->GameObjects[4]))
            {
                game->GameBall.Vel.y = -game->GameBall.Vel.y;
            }

            // Point for right paddle
            if (game->GameBall.Pos.x < 0.0)
            {
                game->GameBall.Pos = {game->Width / 2.0f, game->Height / 2.0f};
                game->GameBall.Vel = {BallSpeed, 0};
                game->GameBall.Rect->x = (i32)game->GameBall.Pos.x;
                game->GameBall.Rect->y = (i32)game->GameBall.Pos.y;
                game->LeftPaddle.Pos = game->Height / 2.0f - PaddleHeightOver2;
                game->RightPaddle.Pos = game->Height / 2.0f - PaddleHeightOver2;
                game->RightPaddle.Score++;
                UpdateScoreText(game, game->Renderer, game->Font);
                game->State = GameState::ScoredRight;
            }

            // Point for left paddle
            if (game->GameBall.Pos.x > game->Width)
            {
                game->GameBall.Pos = {game->Width / 2.0f, game->Height / 2.0f};
                game->GameBall.Vel = {-BallSpeed, 0};
                game->GameBall.Rect->x = (i32)game->GameBall.Pos.x;
                game->GameBall.Rect->y = (i32)game->GameBall.Pos.y;
                game->LeftPaddle.Pos = game->Height / 2.0f - PaddleHeightOver2;
                game->RightPaddle.Pos = game->Height / 2.0f - PaddleHeightOver2;
                game->LeftPaddle.Score++;
                UpdateScoreText(game, game->Renderer, game->Font);
                game->State = GameState::ScoredLeft;
            }

            // AI
            if (game->GameBall.Pos.x > game->Width / 1.5f)
            {
                if ((game->RightPaddle.Pos + PaddleHeightOver2) < (game->GameBall.Pos.y + 10))
                {
                    game->RightPaddle.Vel = AISpeed;
                }
                else
                {
                    game->RightPaddle.Vel = -AISpeed;
                }
            }
            else
            {
                game->RightPaddle.Vel = 0.0f;
            }
        }
    }
}

void Render(Game *game)
{
    SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->Renderer);
    SDL_SetRenderDrawColor(game->Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(game->Renderer, game->GameObjects, 5);
    SDL_Rect dst = {20, 20, game->LeftScore.Width, game->LeftScore.Height};
    SDL_RenderCopy(game->Renderer, game->LeftScore.Texture, NULL, &dst);
    dst = {game->Width - 40, 20, game->RightScore.Width, game->RightScore.Height};
    SDL_RenderCopy(game->Renderer, game->RightScore.Texture, NULL, &dst);
    if (GameState::Menu == game->State)
    {
        MenuRender(&game->GameMenu, game->Renderer);
    }

    if (GameState::ScoredLeft == game->State)
    {
        i32 width = game->LeftHasScoredServingTowardsLeft.Width;
        i32 height = game->LeftHasScoredServingTowardsLeft.Height;
        dst = {(game->Width - width) / 2, 100, width, height};
        SDL_RenderCopy(game->Renderer, game->LeftHasScoredServingTowardsLeft.Texture, NULL, &dst);
    }
    else if (GameState::ScoredRight == game->State)
    {
        i32 width = game->RightHasScoredServingTowardsRight.Width;
        i32 height = game->RightHasScoredServingTowardsRight.Height;
        dst = {(game->Width - width) / 2, 100, width, height};
        SDL_RenderCopy(game->Renderer, game->RightHasScoredServingTowardsRight.Texture, NULL, &dst);
    }
    else if (GameState::LeftWon == game->State)
    {
        i32 width = game->LeftWon.Width;
        i32 height = game->LeftWon.Height;
        dst = {(game->Width - width) / 2, 100, width, height};
        SDL_RenderCopy(game->Renderer, game->LeftWon.Texture, NULL, &dst);
    }
    else if (GameState::RightWon == game->State)
    {
        i32 width = game->RightWon.Width;
        i32 height = game->RightWon.Height;
        dst = {(game->Width - width) / 2, 100, width, height};
        SDL_RenderCopy(game->Renderer, game->RightWon.Texture, NULL, &dst);
    }

    SDL_RenderPresent(game->Renderer);
}
