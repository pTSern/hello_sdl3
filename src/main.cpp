#include "Game.hpp"
#include <iostream>
#include <ostream>
#include "math/Vec2.hpp"

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

    Vec2 v2_1(1, 2);
    Vec2 v2_2(3, 4);
    v2_2.add(v2_1);

    v2_1.log();
    v2_2.log();

    game.run();

    return 0;
}
