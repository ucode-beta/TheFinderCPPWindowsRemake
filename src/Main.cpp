#include "Game.hpp"

#include <cstdio>
#include <exception>

int main() {
    try {
        finder::Game game;
        return game.run();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "TheFinder failed: %s\n", error.what());
        return 1;
    }
}
