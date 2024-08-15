#pragma once

#include <type_traits>

namespace Tesix {

namespace Axis {
struct HORIZONTAL;
struct VERTICAL;
}; // namespace Axis

template<typename T>
struct is_axis : public std::false_type {};

template<>
struct is_axis<Axis::HORIZONTAL> : public std::true_type {};

template<>
struct is_axis<Axis::VERTICAL> : public std::true_type {};

template<typename T>
concept AnyAxis = is_axis<T>::value;

namespace Direction {
struct TOP;
struct BOTTOM;
struct LEFT;
struct RIGHT;
}; // namespace Direction

template<typename T>
struct is_horizontal_direction : std::false_type {};

template<>
struct is_horizontal_direction<Direction::LEFT> : std::true_type {};

template<>
struct is_horizontal_direction<Direction::RIGHT> : std::true_type {};

template<typename T>
struct is_vertical_direction : public std::false_type {};

template<>
struct is_vertical_direction<Direction::TOP> : public std::true_type {};

template<>
struct is_vertical_direction<Direction::BOTTOM> : public std::true_type {};

template<typename T>
struct is_direction : public std::integral_constant<bool, is_horizontal_direction<T>::value || is_vertical_direction<T>::value> {};

template<typename T>
concept AnyDirection = is_direction<T>::value;

template<AnyAxis axis, AnyDirection dir>
struct is_along_axis : std::false_type {};

template<>
struct is_along_axis<Axis::HORIZONTAL, Direction::LEFT> : std::true_type {};

template<>
struct is_along_axis<Axis::HORIZONTAL, Direction::RIGHT> : std::true_type {};

template<>
struct is_along_axis<Axis::VERTICAL, Direction::TOP> : std::true_type {};

template<>
struct is_along_axis<Axis::VERTICAL, Direction::BOTTOM> : std::true_type {};

template<typename dir, typename axis>
concept AxisDirection = is_direction<dir>::value && is_axis<axis>::value && is_along_axis<axis, dir>::value;

template<typename dir>
concept HorizontalDirection = AxisDirection<dir, Axis::HORIZONTAL>;

template<typename dir>
concept VerticalDirection = AxisDirection<dir, Axis::VERTICAL>;

template<AnyDirection>
struct is_strong_direction : std::false_type {};

template<>
struct is_strong_direction<Direction::TOP> : std::true_type {};

template<>
struct is_strong_direction<Direction::LEFT> : std::true_type {};

template<AnyDirection>
struct direction_info;

template<>
struct direction_info<Direction::TOP> {
    using axis = Axis::VERTICAL;
    using opposite = Direction::BOTTOM;
};

template<>
struct direction_info<Direction::BOTTOM> {
    using axis = Axis::VERTICAL;
    using opposite = Direction::TOP;
};

template<>
struct direction_info<Direction::LEFT> {
    using axis = Axis::HORIZONTAL;
    using opposite = Direction::RIGHT;
};

template<>
struct direction_info<Direction::RIGHT> {
    using axis = Axis::HORIZONTAL;
    using opposite = Direction::LEFT;
};

template<AnyAxis>
struct axis_info;

template<>
struct axis_info<Axis::HORIZONTAL> {
    using strong = Direction::LEFT;
    using weak = Direction::RIGHT;
    using opposite = Axis::VERTICAL;
};

template<>
struct axis_info<Axis::VERTICAL> {
    using strong = Direction::TOP;
    using weak = Direction::BOTTOM;
    using opposite = Axis::HORIZONTAL;
};

template<typename dir, typename axis>
concept OppositeAxisDirection = is_direction<dir>::value && is_axis<axis>::value && !is_along_axis<axis, dir>::value;

template<AnyDirection a, OppositeAxisDirection<typename direction_info<a>::axis> b>
struct Diagonal {
    using vert = std::conditional_t<is_vertical_direction<a>::value, a, b>;
    using hor = std::conditional_t<is_horizontal_direction<b>::value, b, a>;
};

template<typename T>
struct is_diagonal : std::false_type {
};

template<>
struct is_diagonal<Diagonal<Direction::TOP, Direction::LEFT>> : std::true_type {
};

template<>
struct is_diagonal<Diagonal<Direction::TOP, Direction::RIGHT>> : std::true_type {
};

template<>
struct is_diagonal<Diagonal<Direction::BOTTOM, Direction::LEFT>> : std::true_type {
};

template<>
struct is_diagonal<Diagonal<Direction::BOTTOM, Direction::RIGHT>> : std::true_type {
};

template<>
struct is_diagonal<Diagonal<Direction::LEFT, Direction::TOP>> : std::true_type {
};

template<>
struct is_diagonal<Diagonal<Direction::RIGHT, Direction::TOP>> : std::true_type {
};

template<>
struct is_diagonal<Diagonal<Direction::LEFT, Direction::BOTTOM>> : std::true_type {
};

template<>
struct is_diagonal<Diagonal<Direction::RIGHT, Direction::BOTTOM>> : std::true_type {
};

template<typename dir>
concept AnyDiagonal = is_diagonal<dir>::value;

} // namespace Tesix
