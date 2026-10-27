#include "Menu.hpp"

#include "Assets.hpp"

namespace finder {
namespace {

constexpr SDL_Rect LogoRect = {100, 70, 600, 300};
constexpr SDL_Rect PlayRect = {300, 455, 200, 100};
constexpr SDL_Rect QuitRect = {300, 600, 200, 100};

bool contains(const SDL_Rect& rectangle, int x, int y) {
    return x >= rectangle.x && y >= rectangle.y
        && x < rectangle.x + rectangle.w && y < rectangle.y + rectangle.h;
}

void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture,
                 const SDL_Rect& rectangle) {
    if (texture != nullptr)
        SDL_RenderCopy(renderer, texture, nullptr, &rectangle);
}

}

void Menu::draw(SDL_Renderer* renderer, const Assets& assets) {
    drawTexture(renderer, assets.logo(), LogoRect);
    drawTexture(renderer, assets.playButton(), PlayRect);
    drawTexture(renderer, assets.quitButton(), QuitRect);
}

MenuAction Menu::handleClick(const SDL_Event& event) noexcept {
    if (event.type != SDL_MOUSEBUTTONDOWN
        || event.button.button != SDL_BUTTON_LEFT)
        return MenuAction::None;
    if (contains(PlayRect, event.button.x, event.button.y))
        return MenuAction::Play;
    if (contains(QuitRect, event.button.x, event.button.y))
        return MenuAction::Quit;
    return MenuAction::None;
}

}
