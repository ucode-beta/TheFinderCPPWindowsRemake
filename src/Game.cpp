#include "Game.hpp"

#include "Menu.hpp"

#include <SDL_image.h>

#include <cstdio>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

namespace finder {
namespace {

constexpr int ScreenWidth = 800;
constexpr int ScreenHeight = 800;
constexpr Uint32 PlayerMoveInterval = 125;
constexpr Uint32 EnemyMoveInterval = 200;

void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture,
                 const SDL_Rect& rectangle) {
    if (texture != nullptr)
        SDL_RenderCopy(renderer, texture, nullptr, &rectangle);
}

SDL_Rect cellRectangle(Position position, int width, int height) {
    const int left = position.x * ScreenWidth / width;
    const int top = position.y * ScreenHeight / height;
    const int right = (position.x + 1) * ScreenWidth / width;
    const int bottom = (position.y + 1) * ScreenHeight / height;
    return {left, top, right - left, bottom - top};
}

}

Game::Game() : map_(Map::loadFromFile("res/maps/map1.json")) {
    try {
        initialize();
    } catch (...) {
        shutdown();
        throw;
    }
}

Game::~Game() {
    shutdown();
}

void Game::initialize() {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(std::string("SDL initialization failed: ")
                                 + SDL_GetError());
    sdlInitialized_ = true;

    const int imageFlags = IMG_Init(IMG_INIT_PNG);
    imageInitialized_ = imageFlags != 0;
    if ((imageFlags & IMG_INIT_PNG) != IMG_INIT_PNG)
        throw std::runtime_error(std::string("SDL_image initialization failed: ")
                                 + IMG_GetError());

    window_ = SDL_CreateWindow("TheFinder", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, ScreenWidth,
                               ScreenHeight, 0);
    if (window_ == nullptr)
        throw std::runtime_error(std::string("Window creation failed: ")
                                 + SDL_GetError());

    renderer_ = SDL_CreateRenderer(
        window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (renderer_ == nullptr)
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    if (renderer_ == nullptr)
        throw std::runtime_error(std::string("Renderer creation failed: ")
                                 + SDL_GetError());

    assets_.load(renderer_);

#ifdef _WIN32
    if (mciSendStringA(
            "open \"res/sound.mp3\" type mpegvideo alias thefinder_music",
            nullptr, 0, nullptr) == 0) {
        musicOpen_ = true;
        mciSendStringA("play thefinder_music repeat", nullptr, 0, nullptr);
    }
#endif
}

void Game::shutdown() noexcept {
#ifdef _WIN32
    if (musicOpen_) {
        mciSendStringA("stop thefinder_music", nullptr, 0, nullptr);
        mciSendStringA("close thefinder_music", nullptr, 0, nullptr);
        musicOpen_ = false;
    }
#endif
    assets_.clear();
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    if (imageInitialized_) {
        IMG_Quit();
        imageInitialized_ = false;
    }
    if (sdlInitialized_) {
        SDL_Quit();
        sdlInitialized_ = false;
    }
}

void Game::startGame() {
    const Position playerStart{4, 4};
    const Position enemyStart{12, 5};
    if (!map_.isWalkable(playerStart) || !map_.isWalkable(enemyStart))
        throw std::runtime_error("The map does not contain valid start cells.");

    player_.reset(playerStart);
    enemy_.reset(enemyStart);
    for (bool& active : wasteActive_)
        active = true;
    organicRemaining_ = 2;
    metalRemaining_ = 2;
    nextEnemyMove_ = SDL_GetTicks() + EnemyMoveInterval;
    nextPlayerMove_ = 0;
    state_ = GameState::Playing;
    SDL_SetWindowTitle(window_, "TheFinder");
}

void Game::handleEvent(const SDL_Event& event, bool& running, Uint32 now) {
    if (event.type == SDL_QUIT) {
        running = false;
        return;
    }
    if (state_ == GameState::Menu || state_ == GameState::Caught
        || state_ == GameState::Won) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            const MenuAction action = Menu::handleClick(event);
            if (action == MenuAction::Play)
                startGame();
            else if (action == MenuAction::Quit)
                running = false;
        } else if (event.type == SDL_KEYDOWN) {
            const SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_RETURN || key == SDLK_SPACE
                || (state_ != GameState::Menu && key == SDLK_r))
                startGame();
            else if (key == SDLK_ESCAPE) {
                if (state_ == GameState::Menu) {
                    running = false;
                } else {
                    state_ = GameState::Menu;
                    SDL_SetWindowTitle(window_, "TheFinder");
                }
            } else if (state_ == GameState::Menu && key == SDLK_q) {
                running = false;
            }
        }
        return;
    }

    if (event.type != SDL_KEYDOWN)
        return;
    const SDL_Keycode key = event.key.keysym.sym;
    if (key == SDLK_ESCAPE) {
        state_ = GameState::Menu;
        SDL_SetWindowTitle(window_, "TheFinder");
    } else if (now >= nextPlayerMove_) {
        switch (key) {
            case SDLK_LEFT:
            case SDLK_a: movePlayer(SDL_SCANCODE_LEFT, now); break;
            case SDLK_RIGHT:
            case SDLK_d: movePlayer(SDL_SCANCODE_RIGHT, now); break;
            case SDLK_UP:
            case SDLK_w: movePlayer(SDL_SCANCODE_UP, now); break;
            case SDLK_DOWN:
            case SDLK_s: movePlayer(SDL_SCANCODE_DOWN, now); break;
            default: break;
        }
    }
}

void Game::movePlayer(SDL_Scancode key, Uint32 now) {
    if (player_.move(key, map_))
        afterPlayerMove(now);
}

void Game::afterPlayerMove(Uint32 now) {
    nextPlayerMove_ = now + PlayerMoveInterval;
    player_.collectWaste(map_, wasteActive_);
    player_.deliverWaste(map_, organicRemaining_, metalRemaining_);
    if (organicRemaining_ == 0 && metalRemaining_ == 0) {
        setWon();
        return;
    }
    if (player_.position() == enemy_.position())
        setCaught();
}

void Game::setCaught() {
    state_ = GameState::Caught;
    SDL_SetWindowTitle(window_, "Caught - TheFinder");
}

void Game::setWon() {
    state_ = GameState::Won;
    SDL_SetWindowTitle(window_, "You win - TheFinder");
}

void Game::update(Uint32 now) {
    if (state_ != GameState::Playing)
        return;
    if (now >= nextPlayerMove_
        && player_.move(SDL_SCANCODE_UNKNOWN, map_))
        afterPlayerMove(now);
    if (state_ == GameState::Playing && now >= nextEnemyMove_) {
        enemy_.moveToward(map_, player_.position());
        nextEnemyMove_ = now + EnemyMoveInterval;
        if (player_.position() == enemy_.position())
            setCaught();
    }
}

void Game::renderMap() {
    for (int y = 0; y < map_.height(); ++y) {
        for (int x = 0; x < map_.width(); ++x) {
            const Position position{x, y};
            const SDL_Rect cell = cellRectangle(position, map_.width(),
                                                 map_.height());
            drawTexture(renderer_, assets_.tile(map_.tileAt(position)), cell);
        }
    }
}

void Game::renderActors(Uint32 now) {
    static constexpr Position wastePositions[Assets::WasteCount] = {
        {9, 6}, {20, 22}, {3, 20}, {12, 13}
    };
    const int cellWidth = ScreenWidth / map_.width();
    const int cellHeight = ScreenHeight / map_.height();
    for (int i = 0; i < Assets::WasteCount; ++i) {
        if (wasteActive_[i]) {
            SDL_Rect waste = {
                wastePositions[i].x * cellWidth,
                wastePositions[i].y * cellHeight,
                cellWidth,
                cellHeight
            };
            drawTexture(renderer_, assets_.waste(i), waste);
        }
    }

    const SDL_Rect enemyRect = cellRectangle(enemy_.position(), map_.width(),
                                             map_.height());
    const SDL_Rect playerRect = cellRectangle(player_.position(), map_.width(),
                                              map_.height());
    drawTexture(renderer_, assets_.enemy(static_cast<int>((now / 360) % 2)),
                enemyRect);
    drawTexture(renderer_, assets_.player(
                    player_.facing(), static_cast<int>((now / 105)
                                                       % Assets::PlayerFrameCount)),
                playerRect);

    if (player_.carriedWaste() != WasteType::None) {
        const int wasteIndex = player_.carriedWaste() == WasteType::Organic
            ? 0 : 2;
        const SDL_Rect carried = {
            playerRect.x + playerRect.w - 15, playerRect.y, 15, 15
        };
        drawTexture(renderer_, assets_.waste(wasteIndex), carried);
    }
}

void Game::render(Uint32 now) {
    SDL_Rect screen = {0, 0, ScreenWidth, ScreenHeight};
    drawTexture(renderer_, assets_.background(static_cast<int>((now / 130)
                                  % Assets::BackgroundFrameCount)), screen);
    if (state_ == GameState::Menu) {
        Menu::draw(renderer_, assets_);
        return;
    }

    renderMap();
    renderActors(now);
    if (state_ == GameState::Caught || state_ == GameState::Won) {
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer_, 16, 5, 28, 190);
        SDL_RenderFillRect(renderer_, &screen);
        Menu::draw(renderer_, assets_);
    }
}

int Game::run() {
    bool running = true;
    while (running) {
        const Uint32 now = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event))
            handleEvent(event, running, now);
        update(now);
        SDL_RenderClear(renderer_);
        render(now);
        SDL_RenderPresent(renderer_);
        SDL_Delay(16);
    }
    return 0;
}

}
