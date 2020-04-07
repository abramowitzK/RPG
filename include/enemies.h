#pragma once
#include <sprite.h>
const f64 EnemyTickTime = 0.25;
const f32 EnemySpeed = 4.0f;
struct enemy_t
{
    Sprite Graphic;
    Path CurrentPath;
    i32 CurrentIndexInPath;
    u64 LastTickTime;
    void Update(double dt)
    {
        FollowPath(dt);
    }

    void FollowPath(double dt)
    {
        if (CurrentPath.Index < 1)
        {
            return;
        }

        // We haven't finished path
        if (CurrentIndexInPath <= CurrentPath.Index)
        {
            auto currentDest = CurrentPath.Tiles[CurrentIndexInPath];
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
            Graphic.Pos += Vector3(dir * EnemySpeed, 0.0f);
        }
    }
};
