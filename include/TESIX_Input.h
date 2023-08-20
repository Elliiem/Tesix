#pragma once 

//Gets Input and returns it as a TESIX_KeyPress (static)

#include <ncurses.h>

#include "TESIX_KeyPress.h"

class TESIX_Input{
    public:
    TESIX_Input();
    ~TESIX_Input();
    public:
    TESIX_KeyPress GetInput();
    public:

    private:
    WINDOW* input_win;
    bool IsCtrl(std::string&str);
    private:

};