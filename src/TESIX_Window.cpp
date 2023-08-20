#include "TESIX_Window.h"

TESIX_Window::TESIX_Window(uint32_t width, uint32_t height, uint32_t x, uint32_t y){
    win = newwin(height, width, y, x);
    this->width = width;
    this->height = height;
    this->x = x;
    this->y = y;
}


TESIX_Window::~TESIX_Window(){

}


void TESIX_Window::Action(TESIX_KeyPress key){

}


void TESIX_Window::Refresh(){
    wrefresh(win);
}
