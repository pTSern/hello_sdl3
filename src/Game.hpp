#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "Particle.hpp"
#include "Vector2.hpp"

enum class EmitterMode {
    Fountain,
    Explosion,
    Vortex
};

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();

private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void clean();

    void spawnParticles(Vector2 pos, int count);
    SDL_Color getRainbowColor(float progress);

    // SDL Window and Renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isRunning = false;

    // Window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;

    // Simulation parameters
    std::vector<Particle> particles;
    EmitterMode currentMode = EmitterMode::Fountain;
    bool gravityEnabled = true;
    bool windEnabled = false;
    Vector2 gravity = { 0.0f, 400.0f };
    Vector2 wind = { 150.0f, 0.0f };

    // Mouse interaction
    Vector2 mousePos = { 0.0f, 0.0f };
    bool mouseLeftDown = false;
    bool mouseRightDown = false;

    // Color cycling
    float hueCycle = 0.0f;

    // Stats
    float fps = 0.0f;
    float frameTimeMs = 0.0f;
    Uint64 lastFrameTicks = 0;
};
