#include "Font.hpp"
#include "font8x8_basic.h"

void Font::drawText(SDL_Renderer* renderer, const std::string& text, float x, float y, float scale, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    float currentX = x;
    
    for (char c : text) {
        // Space character spacing
        if (c == ' ') {
            currentX += 8.0f * scale;
            continue;
        }
        
        // Ensure character is within printable ASCII range
        int ascii = static_cast<int>(c);
        if (ascii < 0 || ascii >= 128) {
            currentX += 8.0f * scale;
            continue;
        }
        
        const unsigned char* glyph = font8x8_basic[ascii];
        
        // Loop through rows of the glyph
        for (int row = 0; row < 8; ++row) {
            unsigned char rowByte = glyph[row];
            // Loop through columns (bits) of the row byte
            for (int col = 0; col < 8; ++col) {
                // The bit is set (LSB to MSB)
                if (rowByte & (1 << col)) {
                    SDL_FRect pixelRect;
                    pixelRect.x = currentX + col * scale;
                    pixelRect.y = y + row * scale;
                    pixelRect.w = scale;
                    pixelRect.h = scale;
                    SDL_RenderFillRect(renderer, &pixelRect);
                }
            }
        }
        
        // Advance cursor for next character
        currentX += 9.0f * scale; // 8 pixels + 1 pixel spacing
    }
}
