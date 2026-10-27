#ifndef FINDER_GAME_HPP
#define FINDER_GAME_HPP

#include "Assets.hpp"
#include "Enemy.hpp"
#include "Map.hpp"
#include "Player.hpp"

#include <SDL.h>

namespace finder {

enum class GameState {
    Menu,
    Playing,
    Caught,
    Won
};

class Game {
public:
    Game();
    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    int run();

private:
    void initialize();
    void shutdown() noexcept;
    void startGame();
    void handleEvent(const SDL_Event& event, bool& running, Uint32 now);
    void update(Uint32 now);
    void render(Uint32 now);
    void renderMap();
    void renderActors(Uint32 now);
    void movePlayer(SDL_Scancode key, Uint32 now);
    void afterPlayerMove(Uint32 now);
    void setCaught();
    void setWon();

    Map map_;
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    Assets assets_;
    Player player_;
    Enemy enemy_;
    GameState state_ = GameState::Menu;
    bool wasteActive_[Assets::WasteCount] = {true, true, true, true};
    int organicRemaining_ = 2;
    int metalRemaining_ = 2;
    Uint32 nextEnemyMove_ = 0;
    Uint32 nextPlayerMove_ = 0;
    bool sdlInitialized_ = false;
    bool imageInitialized_ = false;
    bool musicOpen_ = false;
};

}

#endif
