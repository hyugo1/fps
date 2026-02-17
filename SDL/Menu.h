// Menu.h

#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Menu {
public:
    Menu(SDL_Renderer* renderer) : renderer(renderer) {
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
    }
    font = TTF_OpenFont("BitcountGridDouble.ttf", 50);
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
    }
}

    ~Menu() {
        if (font) TTF_CloseFont(font);
        TTF_Quit();
    }

    void Render(const std::string& textContent) {
        SDL_Color color = {255, 255, 255, 255}; // white
        SDL_Surface* surface = TTF_RenderText_Solid(font, textContent.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int w, h;
        SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
        SDL_Rect dstRect = { (800 - w)/2, (600 - h)/2, w, h };
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

private:
    SDL_Renderer* renderer;
    TTF_Font* font{nullptr};
};