#pragma once
#include <utils.h>

enum class MapTileFlags
{
    Walkable = 0x0,
    Animated = 0x1,
};

// Represents a tile on a map
struct MapTile
{
    // Various flags for game logic
    MapTileFlags Flags;
    // Current frame in sheet. Will only change if tile is animated
    i32 CurrentFrame;
    // Position including depth on the z axis. Position is measured at bottom left corner
    Vector3 Position;
};
