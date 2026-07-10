#include "Game.hpp"
#include "Font.hpp"
#include <iostream>
#include <ostream>
#include <random>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>

Game::Game() {
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Game::~Game() {
    clean();
}

bool Game::initialize() {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create Window
    window = SDL_CreateWindow(
        "SDL3 Neon Particle Sandbox",
        windowWidth,
        windowHeight,
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create Renderer (let SDL choose the best driver, enable VSync if supported)
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Set blend mode for transparency
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    isRunning = true;
    lastFrameTicks = SDL_GetTicks();
    return true;
}

void Game::run() {
    while (isRunning) {
        // Calculate delta time
        Uint64 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastFrameTicks) / 1000.0f;
        lastFrameTicks = currentTicks;

        // Prevent huge deltaTime spikes (e.g. during window dragging)
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        // Calculate performance stats
        frameTimeMs = deltaTime * 1000.0f;
        if (deltaTime > 0.0f) {
            fps = 0.9f * fps + 0.1f * (1.0f / deltaTime); // Smooth FPS counter
        }

        // Cycle through rainbow hues (rainbow speed: 0.15 cycles per second)
        hueCycle += deltaTime * 0.15f;
        if (hueCycle > 1.0f) hueCycle -= 1.0f;

        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        isRunning = false;
                        break;
                    case SDLK_1:
                        currentMode = EmitterMode::Fountain;
                        break;
                    case SDLK_2:
                        currentMode = EmitterMode::Explosion;
                        break;
                    case SDLK_3:
                        currentMode = EmitterMode::Vortex;
                        break;
                    case SDLK_G:
                        gravityEnabled = !gravityEnabled;
                        break;
                    case SDLK_W:
                        windEnabled = !windEnabled;
                        break;
                    case SDLK_C:
                        particles.clear();
                        break;
                    default:
                        break;
                }
                break;

            case SDL_EVENT_MOUSE_MOTION:
                mousePos.x = event.motion.x;
                mousePos.y = event.motion.y;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouseLeftDown = true;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    mouseRightDown = true;
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouseLeftDown = false;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    mouseRightDown = false;
                }
                break;

            default:
                break;
        }
    }
}

void Game::update(float deltaTime) {
    // Spawn particles if left mouse button is pressed
    if (mouseLeftDown) {
        spawnParticles(mousePos, 15);
    }

    Vector2 currentGravity = gravityEnabled ? gravity : Vector2(0.0f, 0.0f);
    Vector2 currentWind = windEnabled ? wind : Vector2(0.0f, 0.0f);

    // Update and filter active particles
    std::vector<Particle> nextParticles;
    nextParticles.reserve(particles.size() + 20);

    for (auto& particle : particles) {
        // If right mouse is held, attract particles to the cursor (Gravity Well)
        if (mouseRightDown) {
            Vector2 toMouse = mousePos - particle.position;
            float distanceSq = toMouse.lengthSquared();
            if (distanceSq > 10.0f) {
                float distance = std::sqrt(distanceSq);
                float forceStrength = 150000.0f / (distanceSq + 500.0f); // Inverse square law approximation
                Vector2 force = (toMouse / distance) * forceStrength;
                particle.velocity += force * deltaTime;
            }
        }

        particle.update(deltaTime, currentGravity, currentWind);

        // Screen boundary collisions with bounce
        float bounce = -0.5f;
        int currentW, currentH;
        SDL_GetWindowSize(window, &currentW, &currentH);

        if (particle.position.x < 0.0f) {
            particle.position.x = 0.0f;
            particle.velocity.x *= bounce;
        } else if (particle.position.x > currentW) {
            particle.position.x = static_cast<float>(currentW);
            particle.velocity.x *= bounce;
        }

        if (particle.position.y < 0.0f) {
            particle.position.y = 0.0f;
            particle.velocity.y *= bounce;
        } else if (particle.position.y > currentH) {
            particle.position.y = static_cast<float>(currentH);
            particle.velocity.y *= bounce;
        }

        if (particle.isAlive) {
            nextParticles.push_back(particle);
        }
    }

    particles = std::move(nextParticles);
}

void Game::render() {
    // Clear screen with a dark, sleek midnight color
    SDL_SetRenderDrawColor(renderer, 10, 10, 16, 255);
    SDL_RenderClear(renderer);

    // Draw grid effect in background
    SDL_SetRenderDrawColor(renderer, 25, 25, 35, 255);
    int currentW, currentH;
    SDL_GetWindowSize(window, &currentW, &currentH);
    const int gridSize = 64;
    for (int x = 0; x < currentW; x += gridSize) {
        SDL_RenderLine(renderer, x, 0, x, currentH);
    }
    for (int y = 0; y < currentH; y += gridSize) {
        SDL_RenderLine(renderer, 0, y, currentW, y);
    }

    // Draw gravity well visual if right click is active
    if (mouseRightDown) {
        SDL_SetRenderDrawColor(renderer, 0, 191, 255, 60);
        for (int r = 10; r < 60; r += 15) {
            // Draw concentric rings around cursor
            // SDL3 doesn't have a built-in RenderDrawCircle, but we can draw crosshairs or boxes
            SDL_FRect wellRect = { mousePos.x - r, mousePos.y - r, r * 2.0f, r * 2.0f };
            SDL_RenderRect(renderer, &wellRect);
        }
    }

    // Draw all particles
    for (const auto& particle : particles) {
        particle.draw(renderer);
    }

    // Draw dashboard/UI
    // Side panel backing
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 200);
    SDL_FRect panelRect = { 10.0f, 10.0f, 320.0f, 260.0f };
    SDL_RenderFillRect(renderer, &panelRect);
    SDL_SetRenderDrawColor(renderer, 0, 255, 127, 255); // Emerald accent border
    SDL_RenderRect(renderer, &panelRect);

    // Render Stats and Instructions
    SDL_Color textColor = { 240, 240, 255, 255 };
    SDL_Color highlightColor = { 0, 255, 255, 255 }; // Cyan for active state
    SDL_Color disabledColor = { 100, 100, 120, 255 };

    Font::drawText(renderer, "SDL3 NEON SANDBOX", 20.0f, 20.0f, 1.5f, { 0, 255, 127, 255 });
    
    // Performance
    std::stringstream ss;
    ss << "FPS: " << std::fixed << std::setprecision(1) << fps 
       << " (" << std::setprecision(2) << frameTimeMs << " ms)";
    Font::drawText(renderer, ss.str(), 20.0f, 50.0f, 1.0f, textColor);

    ss.str("");
    ss.clear();
    ss << "Particles: " << particles.size();
    Font::drawText(renderer, ss.str(), 20.0f, 70.0f, 1.0f, textColor);

    // Modes
    std::string modeStr = "Emitter: ";
    if (currentMode == EmitterMode::Fountain) modeStr += "Fountain (1)";
    else if (currentMode == EmitterMode::Explosion) modeStr += "Explosion (2)";
    else if (currentMode == EmitterMode::Vortex) modeStr += "Vortex (3)";
    Font::drawText(renderer, modeStr, 20.0f, 100.0f, 1.0f, highlightColor);

    // Statuses
    Font::drawText(renderer, gravityEnabled ? "Gravity: ON (G)" : "Gravity: OFF (G)", 20.0f, 125.0f, 1.0f, gravityEnabled ? highlightColor : disabledColor);
    Font::drawText(renderer, windEnabled ? "Wind: ON (W)" : "Wind: OFF (W)", 20.0f, 145.0f, 1.0f, windEnabled ? highlightColor : disabledColor);

    // Controls Help
    Font::drawText(renderer, "CONTROLS:", 20.0f, 180.0f, 1.0f, { 255, 165, 0, 255 }); // Orange
    Font::drawText(renderer, "L-Click : Spawn particles", 20.0f, 200.0f, 1.0f, textColor);
    Font::drawText(renderer, "R-Click : Gravity Well", 20.0f, 215.0f, 1.0f, textColor);
    Font::drawText(renderer, "C key   : Clear particles", 20.0f, 230.0f, 1.0f, textColor);
    Font::drawText(renderer, "ESC     : Exit simulation", 20.0f, 245.0f, 1.0f, textColor);

    // Draw emitter target point (crosshair)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 120);
    SDL_RenderLine(renderer, mousePos.x - 8, mousePos.y, mousePos.x + 8, mousePos.y);
    SDL_RenderLine(renderer, mousePos.x, mousePos.y - 8, mousePos.x, mousePos.y + 8);

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

void Game::spawnParticles(Vector2 pos, int count) {
    const float PI = 3.1415926535f;
    std::cout << "Game::spawnParticles >> Count >> " << count << std::endl;
    for (int i = 0; i < count; ++i) {
        // Base random properties
        float randomAngle = static_cast<float>(std::rand()) / RAND_MAX * 2.0f * PI;
        float randomSpeed = 50.0f + static_cast<float>(std::rand()) / RAND_MAX * 200.0f;
        float lifetime = 0.5f + static_cast<float>(std::rand()) / RAND_MAX * 1.5f;
        float size = 2.0f + static_cast<float>(std::rand()) / RAND_MAX * 4.0f;
        
        // Cycle colors
        // Shifting hue with a small random deviation to create colorful gradients
        float localHue = hueCycle + (static_cast<float>(std::rand()) / RAND_MAX * 0.1f - 0.05f);
        if (localHue < 0.0f) localHue += 1.0f;
        if (localHue > 1.0f) localHue -= 1.0f;
        SDL_Color col = getRainbowColor(localHue);

        Vector2 vel;
        switch (currentMode) {
            case EmitterMode::Fountain: {
                // Pointing upwards (-y) with small horizontal spread
                float spread = 0.4f;
                float angle = -PI / 2.0f + (static_cast<float>(std::rand()) / RAND_MAX * spread - spread / 2.0f);
                vel = Vector2(std::cos(angle), std::sin(angle)) * randomSpeed;
                break;
            }
            case EmitterMode::Explosion: {
                // Outward radial expansion
                vel = Vector2(std::cos(randomAngle), std::sin(randomAngle)) * randomSpeed;
                break;
            }
            case EmitterMode::Vortex: {
                // Perpendicular to center direction, spinning
                Vector2 toCenter = pos - mousePos;
                if (toCenter.lengthSquared() < 0.1f) {
                    vel = Vector2(std::cos(randomAngle), std::sin(randomAngle)) * randomSpeed;
                } else {
                    Vector2 normal = Vector2(-toCenter.y, toCenter.x).normalized();
                    vel = (normal * randomSpeed) + (toCenter.normalized() * -30.0f); // Spin + slight drag inwards
                }
                break;
            }
        }

        particles.emplace_back(pos, vel, col, lifetime, size);
    }
}

SDL_Color Game::getRainbowColor(float progress) {
    float h = progress * 360.0f;
    float s = 1.0f;
    float l = 0.6f; // Slightly brighter neon

    float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;

    float r = 0, g = 0, b = 0;
    if (h >= 0 && h < 60) { r = c; g = x; b = 0; }
    else if (h >= 60 && h < 120) { r = x; g = c; b = 0; }
    else if (h >= 120 && h < 180) { r = 0; g = c; b = x; }
    else if (h >= 180 && h < 240) { r = 0; g = x; b = c; }
    else if (h >= 240 && h < 300) { r = x; g = 0; b = c; }
    else if (h >= 300 && h <= 360) { r = c; g = 0; b = x; }

    return SDL_Color{
        static_cast<Uint8>((r + m) * 255.0f),
        static_cast<Uint8>((g + m) * 255.0f),
        static_cast<Uint8>((b + m) * 255.0f),
        255
    };
}
