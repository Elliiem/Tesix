#pragma once

#include "direction.hpp"
#include "style.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <type_traits>

namespace Tesix {

struct Box {
    size_t _x = 0;
    size_t _y = 0;
    size_t _width = 0;
    size_t _height = 0;

    inline bool isInside(const size_t x, const size_t y) {
        const bool x_check = x > _x && x < _x + _width;
        const bool y_check = y > _y && y < _y + _height;

        return x_check && y_check;
    }

    inline bool canDiscard() {
        return _width == 0 || _height == 0;
    }

    template<AnyAxis>
    inline size_t& position();

    template<>
    inline size_t& position<Axis::HORIZONTAL>() {
        return _x;
    }

    template<>
    inline size_t& position<Axis::VERTICAL>() {
        return _y;
    }

    template<AnyAxis>
    inline size_t& thickness();

    template<>
    inline size_t& thickness<Axis::HORIZONTAL>() {
        return _width;
    }

    template<>
    inline size_t& thickness<Axis::VERTICAL>() {
        return _height;
    }

    template<AnyDirection dir>
    inline size_t getSide() {
        using info = direction_info<dir>;
        using axis = info::axis;

        if constexpr(is_strong_direction<dir>::value) {
            return position<axis>();
        } else {
            return position<axis>() + thickness<axis>();
        }
    }
};

template<VerticalDirection vert, HorizontalDirection hor>
inline Box childBox(const Box& parent, const size_t x, const size_t y);

template<>
inline Box childBox<Direction::TOP, Direction::LEFT>(const Box& parent, size_t x, size_t y) {
    assert(parent._width > x && parent._height > y);

    return {._x = parent._x, ._y = parent._y, ._width = x, ._height = y};
}

template<>
inline Box childBox<Direction::TOP, Direction::RIGHT>(const Box& parent, size_t x, size_t y) {
    assert(parent._width > x && parent._height > y);

    return {._x = parent._x + x, ._y = parent._y, ._width = parent._width - x, ._height = y};
}

template<>
inline Box childBox<Direction::BOTTOM, Direction::LEFT>(const Box& parent, size_t x, size_t y) {
    assert(parent._width > x && parent._height > y);

    return {._x = parent._x, ._y = parent._y + y, ._width = x, ._height = parent._height - y};
}

template<>
inline Box childBox<Direction::BOTTOM, Direction::RIGHT>(const Box& parent, size_t x, size_t y) {
    assert(parent._width > x && parent._height > y);

    return {._x = parent._x + x, ._y = parent._y + y, ._width = parent._width - x, ._height = parent._height - y};
}

enum QuadType {
    LEAF,
    BRANCH,
};

struct Leaf {
    Leaf() = default;
    ~Leaf() = default;

    uint64_t _style = Style::DEFAULT_STYLE;
};

struct Quad;

struct Branch {
    Branch() = default;
    ~Branch() = default;

    Quad* _children = nullptr;
};

union QuadValue {
    QuadValue() {
    }

    Leaf leaf;
    Branch branch;
};

struct Quad {
    QuadType _type;
    QuadValue _value;
    Box _area;

    Quad() {
        _type = LEAF;
    }

    inline Leaf& leaf() {
        assert(_type == LEAF);

        return _value.leaf;
    }

    inline const Leaf& leaf() const {
        assert(_type == LEAF);

        return _value.leaf;
    }

    inline Branch& branch() {
        assert(_type == BRANCH);

        return _value.branch;
    }

    inline const Branch& branch() const {
        assert(_type == BRANCH);

        return _value.branch;
    }

    template<AnyDiagonal diagonal>
    inline Quad& child() {
        assert(_type == BRANCH);

        constexpr size_t horizontal_index = std::is_same_v<typename diagonal::hor, Direction::RIGHT>;
        constexpr size_t vertical_index = 2 * std::is_same_v<typename diagonal::vert, Direction::BOTTOM>;

        return branch()._children[vertical_index + horizontal_index];
    }

    template<VerticalDirection vert, HorizontalDirection hor>
    inline Quad& child() {
        return child<Diagonal<vert, hor>>();
    }

    void toLeaf() {
        assert(_type == BRANCH);

        _type = LEAF;

        delete branch()._children;
    }

    void toBranch(const size_t x, const size_t y) {
        assert(_type == LEAF);
        assert(_area._width > x && _area._height > y);

        _type = BRANCH;

        branch()._children = new Quad[4];

        child<Direction::TOP, Direction::LEFT>()._area = childBox<Direction::TOP, Direction::LEFT>(_area, x, y);
        child<Direction::TOP, Direction::RIGHT>()._area = childBox<Direction::TOP, Direction::RIGHT>(_area, x, y);
        child<Direction::BOTTOM, Direction::LEFT>()._area = childBox<Direction::BOTTOM, Direction::LEFT>(_area, x, y);
        child<Direction::BOTTOM, Direction::RIGHT>()._area = childBox<Direction::BOTTOM, Direction::RIGHT>(_area, x, y);
    }

    template<AnyAxis axis>
    struct SidePair {
        Quad& _first;
        Quad& _second;

        template<AxisDirection<axis> dir>
        void moveSide(const size_t i) {
            _first.moveSide<dir>(i);
            _second.moveSide<dir>(i);
        }
    };

    template<AnyDirection dir>
    inline SidePair<typename direction_info<dir>::axis> sidePair() {
        using axis = direction_info<dir>::axis;
        using opposite_axis = axis_info<axis>::opposite;

        return {
            ._first = child<dir, typename axis_info<opposite_axis>::strong>(),
            ._second = child<dir, typename axis_info<opposite_axis>::weak>(),
        };
    }

    template<AnyAxis axis>
    inline size_t getAxis() {
        assert(_type == BRANCH);

        return branch()._children[0]._area.getSide<axis_info<axis>::weak>();
    }

    template<AnyDirection dir>
    void moveSide(const size_t i) {
        using axis = direction_info<dir>::axis;
        using opposite_dir = direction_info<dir>::opposite;

        const size_t far_side = _area.getSide<opposite_dir>();

        _area.thickness<axis>() = std::max<int64_t>(0, far_side - i);
        _area.position<axis>() = std::min(far_side, i);

        if(_type == BRANCH) {
            sidePair<dir>().template moveSide<dir>(i);

            if(i > getAxis<axis>()) {
                sidePair<opposite_dir>().template moveSide<dir>(i);
            }
        }
    }
};

/**
 * @brief Creates a new root quad
 **/
inline Quad rootQuad(const size_t width, const size_t height) {
    Quad quad;

    quad._area = {._x = 0, ._y = 0, ._width = width, ._height = height};

    return quad;
}

struct QuadTree {
    Quad _root;
};

} // namespace Tesix
