#pragma once
#include <vectorMath.h>
#include <sprite.h>
#include <input.h>
#include <map.h>

constexpr float Speed = 8.0f;
constexpr float Accel = 1.0f;
struct Player
{
    Vector2 Position;
    Sprite Graphic;
    Path Path;
    i32 CurrentIndexInPath;

    void Update(double dt, Input const &input)
    {
        FollowPath(dt);
    }

    void FollowPath(double dt)
    {
        if (Path.Index < 1)
        {
            return;
        }

        // We haven't finished path
        if (CurrentIndexInPath <= Path.Index)
        {
            auto currentDest = Path.Tiles[CurrentIndexInPath];
            if (currentDest.Flags != MapTileFlags::Walkable)
            {
                return;
            }

            auto dir = glm::normalize(currentDest.Rect.GetCenter() - Vector2(Graphic.Pos.x, Graphic.Pos.y));
            if (IsPointInRect(currentDest.Rect, Vector2(Graphic.Pos)))
            {
                CurrentIndexInPath++;
                return;
            }
            Graphic.Pos += Vector3(dir * Speed, 0.0f);
        }
    }
};
