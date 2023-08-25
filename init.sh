# Run this once to initialize the development environment

# Clone tree-sitter libraries
cd lib
git clone https://github.com/tree-sitter/tree-sitter-cpp.git
git clone https://github.com/tree-sitter/tree-sitter.git

# Make the build dir and configure cmake
cd ..
mkdir build
cd build
cmake -GNinja ..
