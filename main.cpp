
#include <ncurses.h>
#include <locale.h>

#include <iostream>
#include <fstream>

#include <string>
#include <vector>

#include "TESIX_FileWindow.h"

int main(int argc, char** argv){
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE);

    std::string file_name = "./src/TESIX_File.cpp";
    TESIX_File file(file_name);

    WINDOW* win = newwin(39,200,0,0);
    TESIX_FileWindow file_win(win);

    file_win.SetFile(&file);

    for(int i = 0;i < 136; i++){
        file_win.top = i;
        file_win.Update();
    }

    wgetch(win);
    endwin();
    return 0;
}