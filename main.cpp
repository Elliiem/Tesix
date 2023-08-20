
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "TESIX_FileWindow.h"
#include "TESIX_Window.h"
#include "TESIX_Input.h"

int main(int argc, char** argv){
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE);

    std::string file_path = "test.txt";
    TESIX_File file(file_path);

    TESIX_FileWindow window(200, 50, 0, 0);
    window.SetFile(&file);
    window.UpdateView();

    TESIX_Input input;
    TESIX_KeyPress key;

    while(key.key != "q"){
        key = input.GetInput();
        window.Action(key);
    }

    endwin();

    return 0;
}