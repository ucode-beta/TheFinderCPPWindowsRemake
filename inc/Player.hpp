#ifndef FINDER_PLAYER_HPP
#define FINDER_PLAYER_HPP

#include "Map.hpp"

#include <SDL.h>

#include <vector>

namespace finder {

enum class Facing {
    Right,
    Left,
    Up,
    Down
};

enum class WasteType {
    None,
    Organic,
    Metal
};

class Player {
public:
    Position position() const noexcept;
    WasteType carriedWaste() const noexcept;
    Facing facing() const noexcept;
    void reset(Position position) noexcept;
    bool move(SDL_Scancode key, const Map& map);
    bool collectWaste(const Map& map, bool activeWaste[4]);
    bool deliverWaste(const Map& map, int& organicRemaining,
                      int& metalRemaining);

    static std::vector<Position> reachablePositions(const Map& map,
                                                   Position from);

private:
    static Position nextPosition(const Map& map, Position from,
                                 SDL_Scancode key);

    Position position_{4, 4};
    WasteType carriedWaste_ = WasteType::None;
    Facing facing_ = Facing::Right;
};

}

#endif
