#include <ncurses.h>

#include "TESIX_FileWindow.h"

TESIX_FileWindow::TESIX_FileWindow(WINDOW* win){
    this->win = win; 
    this->width = win->_maxx + 1;
    this->height = win->_maxy + 1;
    this->x = win->_begx;
    this->y = win->_begy;
    top = 0;
}


TESIX_FileWindow::~TESIX_FileWindow(){

}


void TESIX_FileWindow::SetFile(TESIX_File* file){
    this->file = file;
}


void TESIX_FileWindow::Update(){
    wclear(win);
    uint i = 0;
    while (i < height){
        PrintLine(i + top, i);
        i++;
    }
    
    wrefresh(win);

    wmove(win, cur.line, cur.col);
}


void TESIX_FileWindow::PrintLine(uint line, uint y){
    wmove(win, y, 0);

    std::string line_str = file->GetLine(line);
    std::string line_num = std::to_string(line);

    int line_num_size = line_num.length();

    line_num.resize(line_num_size + (4 - line_num_size), ' ');
    line_num.append("| ");

    line_str.insert(0, line_num);

    if(line >= file->GetLen()){
        if(line_str.length() > width){
            line_str.resize(line_str.length() + (width - line_str.length()));
        }
    }

    wprintw(win, line_str.c_str());
}
