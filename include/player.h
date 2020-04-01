#pragma once
#include <vectorMath.h>
#include <sprite.h>
#include <input.h>

constexpr float Speed = 1.0f;

struct Player
{
    Vector2 Position;
    Sprite Graphic;
    void Update(double dt, Input const &input)
    {
        if (input.Keys.IsKeyPressed(Keys::W))
            Position.y -= Speed;
        if (input.Keys.IsKeyPressed(Keys::A))
            Position.x -= Speed;
        if (input.Keys.IsKeyPressed(Keys::D))
            Position.x += Speed;
        if (input.Keys.IsKeyPressed(Keys::S))
            Position.y += Speed;

        Graphic.Pos = Vector3(Position.x - 50, Position.y - 50, 1.0f);
    }
};
