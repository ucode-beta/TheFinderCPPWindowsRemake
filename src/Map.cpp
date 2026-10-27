#include "Map.hpp"

#include <charconv>
#include <cctype>
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <utility>

namespace finder {
namespace {

struct ParsedMap {
    int width;
    int height;
    std::vector<int> tiles;
};

class MapJsonReader {
public:
    explicit MapJsonReader(std::string_view input) : input_(input) {}

    ParsedMap parse() {
        expect('{');
        expectKey("size_x");
        const int width = parseInteger();
        expect(',');
        expectKey("size_y");
        const int height = parseInteger();
        expect(',');
        expectKey("matrix");
        std::vector<int> tiles = parseTiles();
        expect('}');
        skipWhitespace();
        if (position_ != input_.size())
            fail("unexpected content after map");
        if (width <= 0 || height <= 0
            || static_cast<std::size_t>(width)
                > std::numeric_limits<std::size_t>::max()
                    / static_cast<std::size_t>(height)
            || tiles.size() != static_cast<std::size_t>(width)
                    * static_cast<std::size_t>(height))
            fail("map dimensions do not match matrix size");
        return {width, height, std::move(tiles)};
    }

private:
    void skipWhitespace() {
        while (position_ < input_.size()
               && std::isspace(static_cast<unsigned char>(input_[position_])))
            ++position_;
    }

    void expect(char expected) {
        skipWhitespace();
        if (position_ >= input_.size() || input_[position_] != expected)
            fail(std::string("expected '") + expected + "'");
        ++position_;
    }

    void expectKey(std::string_view key) {
        expect('"');
        for (char expected : key) {
            if (position_ >= input_.size() || input_[position_] != expected)
                fail("unexpected map key");
            ++position_;
        }
        expect('"');
        expect(':');
    }

    int parseInteger() {
        skipWhitespace();
        int value = 0;
        const char* begin = input_.data() + position_;
        const char* end = input_.data() + input_.size();
        const auto result = std::from_chars(begin, end, value);
        if (result.ec != std::errc{} || result.ptr == begin)
            fail("expected integer");
        position_ = static_cast<std::size_t>(result.ptr - input_.data());
        return value;
    }

    std::vector<int> parseTiles() {
        expect('[');
        std::vector<int> tiles;
        skipWhitespace();
        if (position_ < input_.size() && input_[position_] == ']') {
            ++position_;
            return tiles;
        }
        for (;;) {
            tiles.push_back(parseInteger());
            skipWhitespace();
            if (position_ < input_.size() && input_[position_] == ']') {
                ++position_;
                return tiles;
            }
            expect(',');
        }
    }

    [[noreturn]] void fail(const std::string& message) const {
        throw std::runtime_error("Invalid map JSON at byte "
                                 + std::to_string(position_) + ": " + message);
    }

    std::string_view input_;
    std::size_t position_ = 0;
};

}

Map::Map(int width, int height, std::vector<int> tiles)
    : width_(width), height_(height), tiles_(std::move(tiles)) {}

Map Map::loadFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Could not open map file: " + path);
    const std::string content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    MapJsonReader reader(content);
    ParsedMap parsed = reader.parse();
    return Map(parsed.width, parsed.height, std::move(parsed.tiles));
}

int Map::width() const noexcept {
    return width_;
}

int Map::height() const noexcept {
    return height_;
}

int Map::tileAt(Position position) const noexcept {
    if (!contains(position))
        return -1;
    return tiles_[static_cast<std::size_t>(position.y) * width_ + position.x];
}

bool Map::contains(Position position) const noexcept {
    return position.x >= 0 && position.y >= 0
        && position.x < width_ && position.y < height_;
}

bool Map::isWalkable(Position position) const noexcept {
    return tileAt(position) != -1;
}

}
