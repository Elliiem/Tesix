#include "tree_sitter/api.h"
#include <ncurses.h>

extern "C" {
TSLanguage* tree_sitter_cpp();
}

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

    std::string file_path = "/home/elliem/Dev/Programs/1st-Party/cpp/Tesix/main.cpp";
    TESIX_File file(file_path);

    TESIX_FileWindow window(200, 50, 0, 0);
    window.SetFile(&file);
    window.UpdateView();

    TESIX_Input input;
    TESIX_KeyPress key;

    while(key.key != "q") {
        key = input.GetInput();

        if(key.key == "q") {
            break;
        }

        window.Action(key);
    }

    endwin();

    // ._. // ;-;

    TESIX_SyntaxTree foo(&file);
    foo.Debug();

    return 0;
}
