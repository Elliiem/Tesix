#include "direction.hpp"
#include "quadtree.hpp"

template<Tesix::AnyDiagonal>
void foo() {
}

int main() {
    foo<Tesix::Diagonal<Tesix::Direction::TOP, Tesix::Direction::LEFT>>();
}
