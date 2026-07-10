#pragma once
#include "Vector2.hpp"
#include <SDL3/SDL.h>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    SDL_Color color;
    float age = 0.0f;
    float lifetime = 1.0f;
    float size = 4.0f;
    float drag = 0.99f;
    bool isAlive = true;

    Particle(Vector2 pos, Vector2 vel, SDL_Color col, float life, float sz)
        : position(pos), velocity(vel), color(col), lifetime(life), size(sz) {}

    void update(float deltaTime, Vector2 gravity, Vector2 wind) {
        // Apply forces
        velocity += (gravity + wind) * deltaTime;
        velocity *= std::pow(drag, deltaTime * 60.0f); // Frame-rate independent drag
        position += velocity * deltaTime;

        // Update age
        age += deltaTime;
        if (age >= lifetime) {
            isAlive = false;
        }
    }

    void draw(SDL_Renderer* renderer) const {
        if (!isAlive) return;

        // Calculate opacity based on age
        float lifeRatio = age / lifetime;
        Uint8 alpha = static_cast<Uint8>((1.0f - lifeRatio) * 255.0f);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alpha);
        SDL_FRect rect = { position.x - size / 2.0f, position.y - size / 2.0f, size, size };
        
        // Enable blending for transparency
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &rect);
    }
};
