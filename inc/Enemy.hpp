#ifndef FINDER_ENEMY_HPP
#define FINDER_ENEMY_HPP

#include "Map.hpp"

namespace finder {

class Enemy {
public:
    Position position() const noexcept;
    void reset(Position position) noexcept;
    bool moveToward(const Map& map, Position target);

private:
    Position position_{12, 5};
};

}

#endif
