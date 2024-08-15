#pragma once

#include "quadtree.hpp"
#include <stdexcept>

namespace Tesix {

namespace Debug {

inline const char* quadTypeStr(QuadType type) {
    switch(type) {
        case LEAF:
            return "Leaf";
        case BRANCH:
            return "Branch";
    }
}

template<typename T> inline void printDebug(const T& debugee);

template<> inline void printDebug<Box>(const Box& debugee) {
    printf("Box: {\n    ._x = %lu;\n    ._y = %lu;\n    ._width = %lu;\n    ._height = %lu;\n}\n\n", debugee._x, debugee._y, debugee._width, debugee._height);
}

}

}
