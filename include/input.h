#pragma once
#include <functional>
#include <vector>
#include <SDL.h>
#include <keyState.h>
#include <mouseState.h>

enum Keys
{
    A = SDL_SCANCODE_A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Space = SDL_SCANCODE_SPACE,
};

enum Intent : uint32_t
{
    Shutdown = 0x1,
    Resize = 0x2,
    Escape = 0x4,
    Debug = 0x8,
};

struct Input
{
    KeyState Keys;
    MouseState Mouse;
    Intent Intents;
    int x;
    int y;
};
