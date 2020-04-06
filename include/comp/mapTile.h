#pragma once
#include <utils.h>

enum class MapTileFlags
{
    None = 0x0,
    Walkable = 0x1,
    Animated = 0x2,
};

struct Rectangle
{
    float width;
    float height;
    float x;
    float y;

    Vector2 GetCenter() const
    {
        return {x + (width / 2.0), y + (height / 2.0)};
    }
};
// Represents a tile on a map
struct MapTile
{
    // Various flags for game logic
    MapTileFlags Flags;
    // Current frame in sheet. Will only change if tile is animated
    i32 CurrentFrame;
    // Position including depth on the z axis. Position is measured at bottom left corner
    Rectangle Rect;
    i32 Index;
};

static bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
    float dx = std::abs((rec1.x + rec1.width / 2) - (rec2.x + rec2.width / 2));
    float dy = std::abs((rec1.y + rec1.height / 2) - (rec2.y + rec2.height / 2));
    return (dx <= (rec1.width / 2 + rec2.width / 2)) && ((dy <= (rec1.height / 2 + rec2.height / 2)));
}

// function to find if given point
// lies inside a given rectangle or not.
bool IsPointInRect(Rectangle rect, Vector2 point)
{
    return point.x > rect.x && point.x < (rect.x + rect.width) && point.y > rect.y && point.y < (rect.y + rect.height);
}
