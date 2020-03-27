#include <stdio.h>
#include <utils.h>
#include <string.h>
#include <game.h>
#include <input.h>
#include <platform.h>
#include <imgui.h>

#undef main
bool Running = true;
Game State = {};
int main(int argc, char **argv)
{
	Platform platform = Platform("RPG", 800, 600);
	f64 t = 0.0;
	f64 dt = 1 / 60.0f;
	f64 currentTime = platform.GetTime();
	f64 accumulator = 0.0;
	Input input = {};
	while (Running)
	{
		f64 newTime = platform.GetTime();
		f64 frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		platform.PollInput(input);
		if (input.Intents & Intent::Shutdown)
		{
			break;
		}

		while (accumulator >= dt)
		{
			accumulator -= dt;
			FixedUpdate(&State, dt, &input);
			t += dt;
		}

		Update(&State, frameTime, &input);
		platform.BeginFrame();
		ImGui::Begin("Mouse Pos", 0);
		ImGui::Text("%d", input.Mouse.GetMouseX());
		ImGui::Text("%d", input.Mouse.GetMouseY());
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		Render(&State);
		platform.EndFrame();
	}

	SDL_Quit();
	return 0;
}
