#ifndef FINDER_MENU_HPP
#define FINDER_MENU_HPP

#include <SDL.h>

namespace finder {

class Assets;

enum class MenuAction {
    None,
    Play,
    Quit
};

class Menu {
public:
    static void draw(SDL_Renderer* renderer, const Assets& assets);
    static MenuAction handleClick(const SDL_Event& event) noexcept;
};

}

#endif
