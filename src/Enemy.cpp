#include "Enemy.hpp"

#include "Player.hpp"

#include <queue>
#include <vector>

namespace finder {

Position Enemy::position() const noexcept {
    return position_;
}

void Enemy::reset(Position position) noexcept {
    position_ = position;
}

bool Enemy::moveToward(const Map& map, Position target) {
    if (!map.isWalkable(position_) || !map.isWalkable(target))
        return false;
    if (position_ == target)
        return true;

    const int cellCount = map.width() * map.height();
    const auto indexOf = [&map](Position position) {
        return position.y * map.width() + position.x;
    };
    const auto positionOf = [&map](int index) {
        return Position{index % map.width(), index / map.width()};
    };
    const int start = indexOf(position_);
    const int destination = indexOf(target);
    std::vector<int> previous(static_cast<std::size_t>(cellCount), -2);
    std::queue<int> pending;
    previous[static_cast<std::size_t>(start)] = -1;
    pending.push(start);

    while (!pending.empty()
           && previous[static_cast<std::size_t>(destination)] == -2) {
        const int current = pending.front();
        pending.pop();
        const Position currentPosition = positionOf(current);
        for (const Position next :
             Player::reachablePositions(map, currentPosition)) {
            const int nextIndex = indexOf(next);
            if (previous[static_cast<std::size_t>(nextIndex)] != -2)
                continue;
            previous[static_cast<std::size_t>(nextIndex)] = current;
            pending.push(nextIndex);
        }
    }
    if (previous[static_cast<std::size_t>(destination)] == -2)
        return false;

    int next = destination;
    while (previous[static_cast<std::size_t>(next)] != start)
        next = previous[static_cast<std::size_t>(next)];
    position_ = positionOf(next);
    return position_ == target;
}

}
