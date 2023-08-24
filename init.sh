# Run this once to initialize the development environment

# Make the build dir and configure cmake
mkdir build
cd build
cmake -GNinja ..
cd ..

# Clone tree-sitter libraries
cd lib
git clone https://github.com/tree-sitter/tree-sitter-cpp.git
git clone https://github.com/tree-sitter/tree-sitter.git
cd tree-sitter
make

