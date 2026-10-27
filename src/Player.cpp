#include "Player.hpp"

#include <algorithm>
#include <array>

namespace finder {
namespace {

int tileAt(const Map& map, int x, int y) {
    return map.tileAt({x, y});
}

SDL_Scancode normalizeKey(SDL_Scancode key) {
    switch (key) {
        case SDL_SCANCODE_A: return SDL_SCANCODE_LEFT;
        case SDL_SCANCODE_D: return SDL_SCANCODE_RIGHT;
        case SDL_SCANCODE_W: return SDL_SCANCODE_UP;
        case SDL_SCANCODE_S: return SDL_SCANCODE_DOWN;
        default: return key;
    }
}

}

Position Player::position() const noexcept {
    return position_;
}

WasteType Player::carriedWaste() const noexcept {
    return carriedWaste_;
}

Facing Player::facing() const noexcept {
    return facing_;
}

void Player::reset(Position position) noexcept {
    position_ = position;
    carriedWaste_ = WasteType::None;
    facing_ = Facing::Right;
}

Position Player::nextPosition(const Map& map, Position from,
                              SDL_Scancode key) {
    key = normalizeKey(key);
    int x = from.x;
    int y = from.y;
    const int current = tileAt(map, x, y);
    const int below = tileAt(map, x, y + 1);

    if (current == -1)
        return from;
    if ((below == -1 || below == -2) && current != -2) {
        if (key == SDL_SCANCODE_LEFT && map.isWalkable({x - 1, y}))
            --x;
        else if (key == SDL_SCANCODE_RIGHT && map.isWalkable({x + 1, y}))
            ++x;
        else if (key == SDL_SCANCODE_DOWN && below == -2)
            ++y;
    } else if (current == -2 || below == -2) {
        if (key == SDL_SCANCODE_UP
            && (tileAt(map, x, y - 1) == -2 || tileAt(map, x, y - 1) == 0))
            --y;
        else if (key == SDL_SCANCODE_DOWN && below == -2)
            ++y;
        else if (key == SDL_SCANCODE_LEFT && map.isWalkable({x - 1, y}))
            --x;
        else if (key == SDL_SCANCODE_RIGHT && map.isWalkable({x + 1, y}))
            ++x;
    } else if (below == 0) {
        ++y;
    }
    return {x, y};
}

bool Player::move(SDL_Scancode key, const Map& map) {
    key = normalizeKey(key);
    switch (key) {
        case SDL_SCANCODE_LEFT: facing_ = Facing::Left; break;
        case SDL_SCANCODE_RIGHT: facing_ = Facing::Right; break;
        case SDL_SCANCODE_UP: facing_ = Facing::Up; break;
        case SDL_SCANCODE_DOWN: facing_ = Facing::Down; break;
        default: break;
    }
    const Position next = nextPosition(map, position_, key);
    if (next == position_)
        return false;
    position_ = next;
    return true;
}

std::vector<Position> Player::reachablePositions(const Map& map,
                                                Position from) {
    constexpr std::array<SDL_Scancode, 5> controls = {
        SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
        SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT
    };
    std::vector<Position> positions;
    positions.reserve(controls.size());
    for (SDL_Scancode control : controls) {
        const Position next = nextPosition(map, from, control);
        if (next != from
            && std::find(positions.begin(), positions.end(), next)
                == positions.end())
            positions.push_back(next);
    }
    return positions;
}

bool Player::collectWaste(const Map& map, bool activeWaste[4]) {
    static constexpr std::array<Position, 4> wastePositions = {{
        {9, 6}, {20, 22}, {3, 20}, {12, 13}
    }};
    static constexpr std::array<WasteType, 4> wasteTypes = {{
        WasteType::Organic, WasteType::Organic,
        WasteType::Metal, WasteType::Metal
    }};

    if (carriedWaste_ != WasteType::None || !map.contains(position_))
        return false;
    for (std::size_t i = 0; i < wastePositions.size(); ++i) {
        if (activeWaste[i] && position_ == wastePositions[i]) {
            activeWaste[i] = false;
            carriedWaste_ = wasteTypes[i];
            return true;
        }
    }
    return false;
}

bool Player::deliverWaste(const Map& map, int& organicRemaining,
                          int& metalRemaining) {
    const int tile = map.tileAt(position_);
    if (tile == 3 && carriedWaste_ == WasteType::Organic
        && organicRemaining > 0) {
        --organicRemaining;
        carriedWaste_ = WasteType::None;
        return true;
    }
    if (tile == 4 && carriedWaste_ == WasteType::Metal
        && metalRemaining > 0) {
        --metalRemaining;
        carriedWaste_ = WasteType::None;
        return true;
    }
    return false;
}

}
