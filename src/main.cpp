#include "Game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Prevent compiler warnings about unused variables
    (void)argc;
    (void)argv;

#if DEBUG
    std::cout << "OK THIS IS DEBUG" << std::endl;
#endif

#if RELEASE
    std::cout << "OK THIS IS RELEASE" << std::endl;
#endif

#if THAU
    std::cout << "ZZ" << std::endl;
#endif

    Game game;
    if (!game.initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }

    game.run();

    return 0;
}
