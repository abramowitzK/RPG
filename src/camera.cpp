#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/transform.hpp>
#include <camera.h>
#include <glew.h>

constexpr auto MIN_ZOOM = 0.1;
constexpr auto MAX_ZOOM = 50.0;
constexpr auto MAX_SPEED = 50;
constexpr auto ZOOM_FACTOR = 0.10;

Camera::Camera() : Width(0), Height(0), Scale(1.0f), Speed(1000.0), View({})
{
    Position = {0, 0};
    Projection = {};
}

Camera::Camera(int w, int h) : Width(w), Height(h), Scale(1.0f), Speed(1000.0), View({})
{
    Position = {0, 0};
    Projection = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -0.1f, 1000.0f);
}

void Camera::Update(double dt, Input const &input)
{
    if (input.Mouse.HasScrolled())
    {
        auto amt = input.Mouse.GetScrollAmount() * ZOOM_FACTOR;
        Scale += amt;
        if (Scale <= 0.001f)
        {
            Scale = 0.001f;
        }
        if (Scale >= 20.0f)
        {
            Scale = 20.0f;
        }
    }

    if (input.Intents & Intent::Resize)
    {
        Resize(input.x, input.y);
    }
}

void Camera::Render()
{
    //Camera Translation
    Vector3 translate(Scale * -Position.x + Width / 2.0f, Scale * -Position.y + Height / 2.0f, 0.0f);
    View = glm::translate(Projection, translate);

    //Camera Scale
    Vector3 scale(Scale, Scale, 0.0f);
    View = glm::scale(View, scale);
}

Vector2 Camera::ConvertScreenToWorld(glm::vec2 screenCoords) const
{
    // Invert Y direction
    screenCoords.y = Height - screenCoords.y;
    // Make it so that 0 is the center
    screenCoords -= Vector2(Width / 2.0f, Height / 2.0f);
    // Translate with the camera position
    screenCoords += Position;
    // Scale the coordinates
    screenCoords /= Scale;
    return screenCoords;
}

void Camera::Resize(int w, int h)
{
    Width = w;
    Height = h;
    Projection = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
    glViewport(0, 0, w, h);
}
