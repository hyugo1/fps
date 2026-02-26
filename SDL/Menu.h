// Menu.h

#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <cstdio>
#include <functional>

class Menu {
public:
    using MouseStateProvider = std::function<Uint32(int*, int*)>;

    enum MainMenuAction {
        NONE,
        START,
        OPTIONS,
        EXIT
    };

    enum OptionMenuAction {
        DEFAULT,
        EASY, 
        MEDIUM,
        HARD
    };

    Menu(SDL_Renderer* renderer, MouseStateProvider mouseStateProvider = SDL_GetMouseState)
        : renderer(renderer), mouseStateProvider(mouseStateProvider) {
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

    void Render(const std::string& textContent, int screenWidth, int screenHeight) {
        SDL_Color color = {255, 255, 255, 255}; // white
        SDL_Surface* surface = TTF_RenderText_Solid(font, textContent.c_str(), color);
        if (!surface) return;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            return;
        }

        int w, h;
        SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_Rect dstRect = { (screenWidth - w)/2, (screenHeight - h)/2, w, h };
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }


    MainMenuAction UpdateMainMenu(int screenWidth, int screenHeight) { 
        int mouseX = 0;
        int mouseY = 0;
        Uint32 mouseState = mouseStateProvider(&mouseX, &mouseY);
        bool leftDown = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        SDL_Rect startRect;
        SDL_Rect optionsRect;
        SDL_Rect exitRect;
        BuildMainMenuButtonRects(screenWidth, screenHeight, startRect, optionsRect, exitRect);

        MainMenuAction action = NONE;
        bool justPressed = leftDown && !mouseDownLastFrame;
        if (justPressed) {
            if (PointsAreOnSquare(mouseX, mouseY, startRect)) {
                action = START;
            } else if (PointsAreOnSquare(mouseX, mouseY, optionsRect)) {
                action = OPTIONS;
            } else if (PointsAreOnSquare(mouseX, mouseY, exitRect)) {
                action = EXIT;
            }
        }
        mouseDownLastFrame = leftDown;
        return action;
    }
    
    void RenderMainMenu(int screenWidth, int screenHeight) {
        int mouseX = 0;
        int mouseY = 0;
        mouseStateProvider(&mouseX, &mouseY);
        SDL_Rect startRect;
        SDL_Rect optionsRect;
        SDL_Rect exitRect;
        BuildMainMenuButtonRects(screenWidth, screenHeight, startRect, optionsRect, exitRect);
        RenderButton("Start Game", startRect, PointsAreOnSquare(mouseX, mouseY, startRect));
        RenderButton("Options", optionsRect, PointsAreOnSquare(mouseX, mouseY, optionsRect));
        RenderButton("Exit Game", exitRect, PointsAreOnSquare(mouseX, mouseY, exitRect));
    }


    OptionMenuAction UpdateOptionsMenu(int screenWidth, int screenHeight) { 
        int mouseX = 0;
        int mouseY = 0;
        Uint32 mouseState = mouseStateProvider(&mouseX, &mouseY);
        bool leftDown = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        SDL_Rect easyRect;
        SDL_Rect mediumRect;
        SDL_Rect hardRect;
        BuildOptionMenuButtonRects(screenWidth, screenHeight, easyRect, mediumRect, hardRect);
        OptionMenuAction action = DEFAULT;
        bool justPressed = leftDown && !mouseDownLastFrame;
        if (justPressed) {
            if (PointsAreOnSquare(mouseX, mouseY, easyRect)) {
                action = EASY;
            } else if (PointsAreOnSquare(mouseX, mouseY, mediumRect)) {
                action = MEDIUM;
            } else if (PointsAreOnSquare(mouseX, mouseY, hardRect)) {
                action = HARD;
            }
        }
        mouseDownLastFrame = leftDown;
        return action;
    }

    void RenderOptionsMenu(int screenWidth, int screenHeight) {
        int mouseX = 0;
        int mouseY = 0;
        mouseStateProvider(&mouseX, &mouseY);
        SDL_Rect easyRect;
        SDL_Rect mediumRect;
        SDL_Rect hardRect;
        BuildOptionMenuButtonRects(screenWidth, screenHeight, easyRect, mediumRect, hardRect);
        RenderButton("Easy", easyRect, PointsAreOnSquare(mouseX, mouseY, easyRect));
        RenderButton("Medium", mediumRect, PointsAreOnSquare(mouseX, mouseY, mediumRect));
        RenderButton("Hard", hardRect, PointsAreOnSquare(mouseX, mouseY, hardRect));
    }

private:
    bool PointsAreOnSquare(int x, int y, const SDL_Rect &rect) {
        return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
    }

    void RenderButton(const std::string& textContent, const SDL_Rect &rect, bool hover) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        if (hover) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 220);
        } else {
            SDL_SetRenderDrawColor(renderer, 45, 45, 45, 200);
        }
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_Color color = hover ? SDL_Color{255, 255, 120, 255} : SDL_Color{255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(font, textContent.c_str(), color);
        if (!surface) return;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            return;
        }

        int w, h;
        SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
        SDL_Rect dstRect = { rect.x + (rect.w - w)/2, rect.y + (rect.h - h)/2, w, h };
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    void BuildMainMenuButtonRects(int screenWidth, int screenHeight, SDL_Rect &startRect, SDL_Rect &optionsRect, SDL_Rect &exitRect) {
        int buttonWidth = 200;
        int buttonHeight = 60;
        int spacing = 20;
        int totalHeight = 3 * buttonHeight + 2 * spacing;
        int startY = (screenHeight - totalHeight) / 2;

        startRect = { (screenWidth - buttonWidth) / 2, startY, buttonWidth, buttonHeight };
        optionsRect = { (screenWidth - buttonWidth) / 2, startY + buttonHeight + spacing, buttonWidth, buttonHeight };
        exitRect = { (screenWidth - buttonWidth) / 2, startY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight };
    }


    void BuildOptionMenuButtonRects(int screenWidth, int screenHeight, SDL_Rect &easyRect, SDL_Rect &mediumRect, SDL_Rect &hardRect) {
        int buttonWidth = 200;
        int buttonHeight = 60;
        int spacing = 20;
        int totalHeight = 3 * buttonHeight + 2 * spacing;
        int startY = (screenHeight - totalHeight) / 2;

        easyRect = { (screenWidth - buttonWidth) / 2, startY, buttonWidth, buttonHeight };
        mediumRect = { (screenWidth - buttonWidth) / 2, startY + buttonHeight + spacing, buttonWidth, buttonHeight };
        hardRect = { (screenWidth - buttonWidth) / 2, startY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight };
    }

    SDL_Renderer* renderer;
    TTF_Font* font{nullptr};
    bool mouseDownLastFrame{false};
    MouseStateProvider mouseStateProvider;
};