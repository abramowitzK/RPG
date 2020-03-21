#pragma once
#include <memory.h>
#include <utils.h>
constexpr int Size = 512;
struct KeyState
{
    u8 Current[Size];
    u8 Previous[Size];
};

struct MouseState
{
    i32 X;
    i32 Y;
    i32 XPrev;
    i32 YPrev;
};

bool KeyPressedThisFrame(KeyState *state, int code)
{
    if (code < Size)
    {
        u8 pressedThisFrame = state->Current[code];
        u8 pressedLastFrame = state->Previous[code];
        return pressedThisFrame && !pressedLastFrame;
    }

    return false;
}

void SetKeyDown(KeyState *state, int code)
{
    if (code < Size)
    {
        state->Current[code] = true;
    }
}

void SetKeyUp(KeyState *state, int code)
{
    if (code < Size)
    {
        state->Current[code] = false;
    }
}

bool IsKeyPressed(KeyState *state, int code)
{
    if (code < Size)
    {
        return state->Current[code];
    }

    return false;
}

void UpdatePrevious(KeyState *state)
{
    memcpy(state->Previous, state->Current, Size);
    memset(state->Current, 0, Size);
}

void MouseSetPosition(MouseState *state, const int x, const int y)
{
    state->X = x;
    state->Y = y;
}

int MouseX(MouseState *state)
{
    return state->X;
}

int MouseY(MouseState *state)
{
    return state->Y;
}

void MouseUpdatePrevious(MouseState *state)
{
    state->XPrev = state->X;
    state->YPrev = state->Y;
    state->X = 0;
    state->Y = 0;
}