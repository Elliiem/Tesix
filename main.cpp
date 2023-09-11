#include <ncurses.h>
#include "tree_sitter/api.h"

extern "C"{
    TSLanguage *tree_sitter_cpp();
}

#include <locale>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <optional>
#include <chrono>

#include "TESIX_File.h"
#include "TESIX_FileWindow.h"
#include "TESIX_Input.h"
#include "TESIX_SyntaxTree.h"
#include "TESIX_ColoredString.h"

int main(int argc, char** argv){
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE);

    std::string file_path = "/home/elliem/Dev/Programs/1st-Party/cpp/Tesix/main.cpp";
    TESIX_File file(file_path);

    TESIX_FileWindow window(200, 50, 0, 0);
    window.SetFile(&file);
    window.UpdateView();

    TESIX_Input input;
    TESIX_KeyPress key;

    while(key.key != "q"){
        key = input.GetInput();
        if(key.key == "q"){
            break;
        }
        window.Action(key);
    }

    endwin();

    /*
    °v°
    rawr
    rawr
    rawr
    rawr
    ^v^
    */

   // ._. // ;-;

    TESIX_SyntaxTree foo(&file);

    foo.TEST();

    std::cout << "\n";
    for(TSNode node : foo.errors){
        std::cout << ts_node_type(node) << "\n";
    }

    return 0;
}
