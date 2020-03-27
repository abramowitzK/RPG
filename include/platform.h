#pragma once
#include <SDL.h>
#include <SDL_filesystem.h>
#include <stdio.h>
#include <type_traits>
#include <string>
#include <input.h>

struct Platform
{
public:
    Platform(const char *title, int width, int height);
    ~Platform();
    void MakeWindow();
    void HandleResize(int width, int height);
    double GetTime();
    void SwapBuffers() const;
    void CaptureMouse(bool capture);
    void PollInput(Input &input);
    void BeginFrame() const;
    void EndFrame() const;

private:
    SDL_Window *m_windowHandle;
    SDL_GLContext m_context;
    double m_frequency;
    Uint64 m_clockStart;
};
