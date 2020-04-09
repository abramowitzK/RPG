#pragma once
#include <utils.h>
#include <sprite.h>
const f32 BulletSpeed = 10.0f;
const f32 BulletTimeToLive = 2.0f;
struct bullet_t {
    f64 StartTime;
    Sprite Graphic;
    Vector2 Direction;
};
