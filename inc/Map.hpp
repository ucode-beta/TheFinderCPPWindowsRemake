#ifndef FINDER_MAP_HPP
#define FINDER_MAP_HPP

#include <string>
#include <vector>

namespace finder {

struct Position {
    int x;
    int y;

    bool operator==(const Position& other) const noexcept {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const noexcept {
        return !(*this == other);
    }
};

class Map {
public:
    static Map loadFromFile(const std::string& path);

    int width() const noexcept;
    int height() const noexcept;
    int tileAt(Position position) const noexcept;
    bool contains(Position position) const noexcept;
    bool isWalkable(Position position) const noexcept;

private:
    Map(int width, int height, std::vector<int> tiles);

    int width_;
    int height_;
    std::vector<int> tiles_;
};

}

#endif
