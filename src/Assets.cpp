#include "Assets.hpp"

#include <SDL_image.h>

#include <string>
#include <stdexcept>

namespace finder {

Assets::~Assets() {
    clear();
}

SDL_Texture* Assets::loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (texture == nullptr)
        throw std::runtime_error(std::string("Could not load ") + path
                                 + ": " + IMG_GetError());
    return texture;
}

void Assets::load(SDL_Renderer* renderer) {
    for (int i = 0; i < BackgroundFrameCount; ++i) {
        const std::string path = "res/GIF" + std::to_string(i + 1) + ".png";
        backgrounds_[static_cast<std::size_t>(i)] =
            loadTexture(renderer, path.c_str());
    }

    logo_ = loadTexture(renderer, "res/logo.bmp");
    playButton_ = loadTexture(renderer, "res/play1.bmp");
    quitButton_ = loadTexture(renderer, "res/quit1.bmp");
    wall_ = loadTexture(renderer, "res/texture/blockofplatform.png");
    ladder_ = loadTexture(renderer, "res/texture/download.png");
    floor_ = loadTexture(renderer, "res/texture/space.png");
    organicBin_ = loadTexture(renderer, "res/trash/trashbox1.png");
    metalBin_ = loadTexture(renderer, "res/trash/trashbox2.png");
    waste_[0] = loadTexture(renderer, "res/trash/trash1.png");
    waste_[1] = loadTexture(renderer, "res/trash/trash2.png");
    waste_[2] = loadTexture(renderer, "res/trash/trash4.png");
    waste_[3] = loadTexture(renderer, "res/trash/trash5.png");
    enemies_[0] = loadTexture(renderer, "res/enemy/enemies1.png");
    enemies_[1] = loadTexture(renderer, "res/enemy/enemies2.png");

    for (int i = 0; i < PlayerFrameCount; ++i) {
        const std::string right =
            "res/player/right/moveright" + std::to_string(i + 1) + ".png";
        const std::string left =
            "res/player/left/moveleft" + std::to_string(i + 1) + ".png";
        playerRight_[static_cast<std::size_t>(i)] =
            loadTexture(renderer, right.c_str());
        playerLeft_[static_cast<std::size_t>(i)] =
            loadTexture(renderer, left.c_str());
    }
    for (int i = 0; i < 2; ++i) {
        const std::string up =
            "res/player/up/moveup" + std::to_string(i + 1) + ".png";
        playerUp_[static_cast<std::size_t>(i)] =
            loadTexture(renderer, up.c_str());
    }
}

void Assets::clear() noexcept {
    for (SDL_Texture*& texture : backgrounds_) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    for (SDL_Texture*& texture : waste_) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    for (SDL_Texture*& texture : playerRight_) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    for (SDL_Texture*& texture : playerLeft_) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    for (SDL_Texture*& texture : playerUp_) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    for (SDL_Texture*& texture : enemies_) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    SDL_DestroyTexture(logo_);
    SDL_DestroyTexture(playButton_);
    SDL_DestroyTexture(quitButton_);
    SDL_DestroyTexture(wall_);
    SDL_DestroyTexture(ladder_);
    SDL_DestroyTexture(floor_);
    SDL_DestroyTexture(organicBin_);
    SDL_DestroyTexture(metalBin_);
    logo_ = nullptr;
    playButton_ = nullptr;
    quitButton_ = nullptr;
    wall_ = nullptr;
    ladder_ = nullptr;
    floor_ = nullptr;
    organicBin_ = nullptr;
    metalBin_ = nullptr;
}

SDL_Texture* Assets::background(int frame) const noexcept {
    return backgrounds_[static_cast<std::size_t>(frame)
                        % backgrounds_.size()];
}

SDL_Texture* Assets::logo() const noexcept {
    return logo_;
}

SDL_Texture* Assets::playButton() const noexcept {
    return playButton_;
}

SDL_Texture* Assets::quitButton() const noexcept {
    return quitButton_;
}

SDL_Texture* Assets::tile(int tileType) const noexcept {
    switch (tileType) {
        case -2: return ladder_;
        case -1: return wall_;
        case 0: return floor_;
        case 3: return organicBin_;
        case 4: return metalBin_;
        default: return nullptr;
    }
}

SDL_Texture* Assets::waste(int index) const noexcept {
    if (index < 0 || index >= WasteCount)
        return nullptr;
    return waste_[static_cast<std::size_t>(index)];
}

SDL_Texture* Assets::player(Facing facing, int frame) const noexcept {
    const std::size_t index = static_cast<std::size_t>(frame)
                              % PlayerFrameCount;
    switch (facing) {
        case Facing::Left: return playerLeft_[index];
        case Facing::Up:
        case Facing::Down:
            return playerUp_[index % playerUp_.size()];
        case Facing::Right:
        default:
            return playerRight_[index];
    }
}

SDL_Texture* Assets::enemy(int frame) const noexcept {
    return enemies_[static_cast<std::size_t>(frame) % enemies_.size()];
}

}
