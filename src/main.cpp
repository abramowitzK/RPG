#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <utils.h>
#include <string.h>
#include <vector_math.h>
#include <game.h>
#include <input.h>

#undef main
bool Running = true;
Game PongGame = {};

int initialize(u32 width, u32 height)
{
	Initialize(&PongGame, height, width);
	return 0;
}

int main(int argc, char **argv)
{
	if (initialize(800, 600))
	{
		return -1;
	}

	f64 t = 0.0;
	f64 dt = 0.01;
	f64 currentTime = SDL_GetTicks();
	f64 accumulator = 0.0;
	SDL_Event event;
	KeyState keys = {};
	MouseState mouse = {};
	while (Running)
	{
		f64 newTime = SDL_GetTicks();
		f64 frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				Running = false;
			}
		}

		if (!Running)
		{
			break;
		}

		i32 numKeys = 0;
		const u8 *sdlKeys = SDL_GetKeyboardState(&numKeys);
		memcpy(keys.Current, sdlKeys, numKeys);
		SDL_GetMouseState(&mouse.X, &mouse.Y);
		while (accumulator >= dt)
		{
			accumulator -= dt;
			FixedUpdate(&PongGame, dt, &keys, &mouse);
			t += dt;
		}

		Update(&PongGame, frameTime, &keys, &mouse);
		Render(&PongGame);
		UpdatePrevious(&keys);
		MouseUpdatePrevious(&mouse);
		Running = (PongGame.State != GameState::Quit);
	}

	if (PongGame.Renderer)
	{
		SDL_DestroyRenderer(PongGame.Renderer);
	}

	if (PongGame.Window)
	{
		SDL_DestroyWindow(PongGame.Window);
	}

	SDL_Quit();
	return 0;
}