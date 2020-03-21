#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector_math.h>
struct RenderableText
{
    SDL_Texture *Texture;
    i32 Width;
    i32 Height;
};

RenderableText GetTextureFromString(SDL_Renderer *renderer, const char *string, SDL_Color color, TTF_Font *font)
{
    RenderableText text = {};
    SDL_Surface *surface = TTF_RenderText_Blended(font, string, color);
    text.Texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    TTF_SizeText(font, string, &text.Width, &text.Height);
    return text;
}