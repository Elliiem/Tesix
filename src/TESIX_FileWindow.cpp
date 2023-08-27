#include <ncurses.h>

#include "TESIX_FileWindow.h"
#include "TESIX_Editor.h"

const uint32_t UINT_NEG_MIN = 4000000000;

TESIX_FileWindow::TESIX_FileWindow(uint32_t width, uint32_t height, uint32_t x, uint32_t y) : TESIX_Window(width, height, x, y){
    top = 0;
    left = 0;
}


TESIX_FileWindow::~TESIX_FileWindow(){

}


void TESIX_FileWindow::SetFile(TESIX_File* file){
    this->file = file;
    top = 0;
    left = 0;
}


void TESIX_FileWindow::Action(TESIX_KeyPress key){

}


void TESIX_FileWindow::SetCur(TESIX_Location loc){
    cur.line = loc.line;
    if(cur.line > UINT_NEG_MIN) cur.line = 0;
    if(cur.line >= top + height){
        top = cur.line;
    }else if(cur.line < top){
        top = cur.line;
    }

    cur.col = loc.col;
    if(cur.col > UINT_NEG_MIN) cur.col = 0;
    if(cur.col < left){
        left = cur.col;
    }else if(cur.col + 6 >= left + width){
        left = (cur.col - width) + 7;
    }
}


void TESIX_FileWindow::SetCur(uint32_t line, uint32_t col){
    cur.line = line;
    if(cur.line > UINT_NEG_MIN) cur.line = 0;
    if(cur.line >= top + height){
        top += cur.line - height;
    }else if(cur.line < top){
        top = cur.line;
    }

    cur.col = col;
    if(cur.col > UINT_NEG_MIN) cur.col = 0;
    if(cur.col < left){
        left = cur.col;
    }else if(cur.col + 6 >= left + width){
        left = (cur.col - width) + 7;
    }
}


void TESIX_FileWindow::MoveCur(uint32_t line, uint32_t col){
    cur.line += line;
    if(cur.line > UINT_NEG_MIN) cur.line = 0;
    if(cur.line >= top + height){
        top += line;
    }else if(cur.line < top){
        top += line;
    }

    cur.col += col;
    if(cur.col > UINT_NEG_MIN) cur.col = 0;
    if(cur.col < left){
        left += col;
    }else if(cur.col + 6 >= left + width){
        left += col;
    }
}
