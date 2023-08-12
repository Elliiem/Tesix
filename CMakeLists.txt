cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tesix)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(tesix
./main.cpp
./src/TESIX_File.cpp
./src/TESIX_Selection.cpp
./src/TESIX_FileWindow.cpp
)

set_target_properties(tesix PROPERTIES OUTPUT_NAME "out")

target_include_directories(tesix PUBLIC
./include
)

target_link_libraries(tesix ${CURSES_LIBRARIES})