#pragma once
#include <string>
#include <SDL3/SDL.h>

class Font {
public:
    // Render text onto the given renderer
    static void drawText(SDL_Renderer* renderer, const std::string& text, float x, float y, float scale, SDL_Color color);
};
