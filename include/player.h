#pragma once
#include <vectorMath.h>
#include <sprite.h>
#include <input.h>

constexpr float Speed = 8.0f;
constexpr float Accel = 1.0f;
struct Player
{
    Vector2 Position;
    Sprite Graphic;
    void Update(double dt, Input const &input)
    {
        if (input.Keys.IsKeyPressed(Keys::W))
            Graphic.Pos.y -= Speed;
        if (input.Keys.IsKeyPressed(Keys::A))
            Graphic.Pos.x -= Speed;
        if (input.Keys.IsKeyPressed(Keys::D))
            Graphic.Pos.x += Speed;
        if (input.Keys.IsKeyPressed(Keys::S))
            Graphic.Pos.y += Speed;
    }
};
