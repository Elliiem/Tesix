#include "tree_sitter/api.h"
#include <ncurses.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <optional>
#include <string>
#include <unistd.h>
#include <vector>

#include "TESIX_ColoredString.h"
#include "TESIX_File.h"
#include "TESIX_FileWindow.h"
#include "TESIX_Input.h"
#include "TESIX_SyntaxTree.h"

int main(int argc, char** argv) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    std::string file_path = "/home/elliem/Dev/Programs/1st-Party/cpp/Tesix/src/TESIX_SyntaxTree.cpp";
    TESIX_File file(file_path);

    TESIX_FileWindow window(200, 50, 0, 0);
    window.SetFile(&file);
    window.UpdateView();

    TESIX_Input input;
    TESIX_KeyPress key;

    while (key.key != "q") {
        key = input.GetInput();

        if (key.key == "q") {
            break;
        }

        window.Action(key);
    }

    endwin();

    // ._. // ;-;

    TESIX_SyntaxTree foo(&file);
    /*uint32_t sum = 0;
    uint32_t count = 10;

    for (uint32_t i = 0; i < count; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        foo.Debug();
        auto end = std::chrono::high_resolution_clock::now();
        sum += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    std::cout << sum / count << "\n";*/

    foo.Debug();

    return 0;
}
