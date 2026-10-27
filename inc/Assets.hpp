#ifndef FINDER_ASSETS_HPP
#define FINDER_ASSETS_HPP

#include "Player.hpp"

#include <SDL.h>

#include <array>

namespace finder {

class Assets {
public:
    static constexpr int BackgroundFrameCount = 12;
    static constexpr int PlayerFrameCount = 8;
    static constexpr int WasteCount = 4;

    Assets() = default;
    ~Assets();
    Assets(const Assets&) = delete;
    Assets& operator=(const Assets&) = delete;

    void load(SDL_Renderer* renderer);
    void clear() noexcept;

    SDL_Texture* background(int frame) const noexcept;
    SDL_Texture* logo() const noexcept;
    SDL_Texture* playButton() const noexcept;
    SDL_Texture* quitButton() const noexcept;
    SDL_Texture* tile(int tileType) const noexcept;
    SDL_Texture* waste(int index) const noexcept;
    SDL_Texture* player(Facing facing, int frame) const noexcept;
    SDL_Texture* enemy(int frame) const noexcept;

private:
    static SDL_Texture* loadTexture(SDL_Renderer* renderer,
                                    const char* path);

    std::array<SDL_Texture*, BackgroundFrameCount> backgrounds_{};
    SDL_Texture* logo_ = nullptr;
    SDL_Texture* playButton_ = nullptr;
    SDL_Texture* quitButton_ = nullptr;
    SDL_Texture* wall_ = nullptr;
    SDL_Texture* ladder_ = nullptr;
    SDL_Texture* floor_ = nullptr;
    SDL_Texture* organicBin_ = nullptr;
    SDL_Texture* metalBin_ = nullptr;
    std::array<SDL_Texture*, WasteCount> waste_{};
    std::array<SDL_Texture*, PlayerFrameCount> playerRight_{};
    std::array<SDL_Texture*, PlayerFrameCount> playerLeft_{};
    std::array<SDL_Texture*, 2> playerUp_{};
    std::array<SDL_Texture*, 2> enemies_{};
};

}

#endif
