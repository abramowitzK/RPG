#pragma once

#include <SDL.h>
#include <ttf.h>
#include <vector_math.h>
enum MenuItems
{
    MenuItemsPlay,
    MenuItemsSettings,
    MenuItemsQuit
};

constexpr i32 MaxMenuOptions = 16;
struct GameMenu
{
    i32 SelectedMenuOption = 0;
    RenderableText MenuOptions[MaxMenuOptions];
    SDL_Rect DestRects[MaxMenuOptions];
    i32 NumMenuOptions = 0;
    TTF_Font *Font;
    i32 ScreenWidth;
    i32 ScreenHeight;
};

GameMenu MenuInitialize(i32 width, i32 height)
{
    GameMenu menu = {};
    menu.Font = TTF_OpenFont("fonts/arial.ttf", 20);
    menu.ScreenWidth = width;
    menu.ScreenHeight = height;
    return menu;
}

void MenuAddOption(GameMenu *menu, SDL_Renderer *renderer, const char *menuOptionText, SDL_Color color)
{
    constexpr i32 Padding = 20;
    u32 heightOfNewOption = Padding;
    for (i32 i = 0; i < menu->NumMenuOptions; ++i)
    {
        heightOfNewOption += menu->MenuOptions[i].Height + Padding;
    }

    RenderableText *menuOption = &menu->MenuOptions[menu->NumMenuOptions];
    *menuOption = GetTextureFromString(renderer, menuOptionText, color, menu->Font);
    i32 textWidth = menuOption->Width;
    i32 textHeight = menuOption->Height;
    SDL_Rect *destRect = &menu->DestRects[menu->NumMenuOptions];
    destRect->x = (menu->ScreenWidth - textWidth) / 2;
    destRect->y = heightOfNewOption;
    destRect->w = textWidth;
    destRect->h = textHeight;
    menu->NumMenuOptions++;
}

void MenuRender(GameMenu *menu, SDL_Renderer *renderer)
{
    for (i32 i = 0; i < menu->NumMenuOptions; ++i)
    {
        SDL_RenderCopy(renderer, menu->MenuOptions[i].Texture, NULL, &menu->DestRects[i]);
    }
}

void MenuUpdate(GameMenu *menu, f64 dt)
{
    for (i32 i = 0; i < menu->NumMenuOptions; ++i)
    {
        SDL_SetTextureColorMod(menu->MenuOptions[i].Texture, 255, 255, 255);
    }

    SDL_SetTextureColorMod(menu->MenuOptions[menu->SelectedMenuOption].Texture, 255, 0, 0);
}
