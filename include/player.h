#pragma once
#include <vectorMath.h>
#include <sprite.h>
#include <input.h>

constexpr float Speed = 600.0f;

struct Player
{
    Vector2 Position;
    Sprite Graphic;
    void Update(double dt, Input const &input)
    {
        if (input.Keys.IsKeyPressed(Keys::W))
            Position.y += (float)dt * Speed;
        if (input.Keys.IsKeyPressed(Keys::A))
            Position.x -= (float)dt * Speed;
        if (input.Keys.IsKeyPressed(Keys::D))
            Position.x += (float)dt * Speed;
        if (input.Keys.IsKeyPressed(Keys::S))
            Position.y -= (float)dt * Speed;

        Graphic.Pos = Vector3(Position.x - 50, Position.y - 50, 1.0f);
    }
};
