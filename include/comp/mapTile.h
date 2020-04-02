#pragma once
#include <utils.h>

enum class MapTileFlags
{
    None = 0x0,
    Walkable = 0x1,
    Animated = 0x2,
};

struct Rectangle {
    float width;
    float height;
    float x;
    float y;
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
};

static bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
    bool collision = false;

    int dx = abs((rec1.x + rec1.width/2) - (rec2.x + rec2.width/2));
    int dy = abs((rec1.y + rec1.height/2) - (rec2.y + rec2.height/2));

    if ((dx <= (rec1.width/2 + rec2.width/2)) && ((dy <= (rec1.height/2 + rec2.height/2)))) collision = true;

    return collision;
}
