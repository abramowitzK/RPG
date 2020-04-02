#include <platform.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glew.h>
#include <input.h>

Platform::Platform(const char *title, int width, int height)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    m_frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    m_clockStart = SDL_GetPerformanceCounter();
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    m_windowHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_context = SDL_GL_CreateContext(m_windowHandle);
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    SDL_GL_SetSwapInterval(1);
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        assert(false);
    }
    ImGui_ImplSDL2_InitForOpenGL(m_windowHandle, m_context);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Platform::~Platform()
{
    SDL_DestroyWindow(m_windowHandle);
    SDL_GL_DeleteContext(m_context);
    SDL_Quit();
}

double Platform::GetTime()
{
    return static_cast<double>(SDL_GetPerformanceCounter() - m_clockStart) / m_frequency;
}

void Platform::SwapBuffers() const
{
    SDL_GL_SwapWindow(m_windowHandle);
}

void Platform::CaptureMouse(bool capture)
{
    if (capture)
        SDL_SetRelativeMouseMode(SDL_TRUE);
    else
        SDL_SetRelativeMouseMode(SDL_FALSE);
}

void Platform::PollInput(Input &input)
{
    ImGuiIO &io = ImGui::GetIO();
    SDL_Event e;
    input.Mouse.SetScrollAmount(0);
    input.Intents = {};
    while (SDL_PollEvent(&e))
    {
        if (io.WantCaptureMouse)
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
        }
        else
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                input.Keys.KeyDown(e.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                input.Keys.KeyUp(e.key.keysym.scancode);
                break;
            case SDL_QUIT:
                input.Intents = Intent::Shutdown;
                break;
            case SDL_MOUSEMOTION:
                input.Mouse.IncrementRelativePosition(e.motion.xrel, e.motion.yrel);
                input.Mouse.SetPosition(e.motion.x, e.motion.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT)
                    input.Mouse.SetLeftButton(true);
                if (e.button.button == SDL_BUTTON_RIGHT)
                    input.Mouse.SetRightButtonDown(true);
                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT)
                    input.Mouse.SetLeftButton(false);
                if (e.button.button == SDL_BUTTON_RIGHT)
                    input.Mouse.SetRightButtonDown(false);
                break;
            case SDL_MOUSEWHEEL:
            {
                input.Mouse.SetScrollAmount(e.wheel.y);
            }
            case SDL_WINDOWEVENT:
                switch (e.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    input.Intents = Intent::Resize;
                    input.x = e.window.data1;
                    input.y = e.window.data2;
                }
                break;
                default:
                    break;
                }
            default:
                break;
            }
        }
    }
}

void Platform::BeginFrame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_windowHandle);
    ImGui::NewFrame();
}

void Platform::EndFrame() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
    SwapBuffers();
}
