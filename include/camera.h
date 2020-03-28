#pragma once
#include <input.h>
#include <vectorMath.h>
struct Camera
{
    Camera();
    Camera(int w, int h);
    void Update(double dt, Input const &input);
    void Render();
    Vector2 ConvertScreenToWorld(glm::vec2 screenCoords) const;
    void Resize(int w, int h);
    Vector2 Position;
    Matrix4 View;
    Matrix4 Projection;
    float Speed;
    float Scale;
    int Width;
    int Height;
};