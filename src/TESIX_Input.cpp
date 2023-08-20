#include "TESIX_Input.h"

#include <iostream>

TESIX_Input::TESIX_Input(){
    input_win = newwin(1,1,0,0);
    keypad(input_win, TRUE);
    raw();
    wtimeout(input_win,100);
    set_escdelay(0);
}

TESIX_Input::~TESIX_Input(){

}

TESIX_KeyPress TESIX_Input::GetInput(){
    TESIX_KeyPress key;

    int raw_input = wgetch(input_win);
    wclear(input_win);
    wmove(input_win,0,0);
    switch (raw_input){
        case KEY_UP:
            key.key = "KEY_UP";
        break;

        case KEY_LEFT:
            key.key = "KEY_LEFT";
        break;

        case KEY_RIGHT:
            key.key = "KEY_RIGHT";
        break;
        
        case KEY_DOWN:
            key.key = "KEY_DOWN";
        break;

        case KEY_BACKSPACE:
            key.key = "KEY_BACKSPACE";
        break;

        case KEY_F(1):
            key.key = "KEY_F1";
        break;

        case KEY_F(2):
            key.key = "KEY_F2";
        break;

        case KEY_F(3):
            key.key = "KEY_F3";
        break;

        case KEY_F(4):
            key.key = "KEY_F4";
        break;

        case KEY_F(5):
            key.key = "KEY_F5";
        break;

        case KEY_F(6):
            key.key = "KEY_F6";
        break;

        case KEY_F(7):
            key.key = "KEY_F7";
        break;

        case KEY_F(8):
            key.key = "KEY_F8";
        break;

        case KEY_F(9):
            key.key = "KEY_F9";
        break;

        case KEY_F(10):
            key.key = "KEY_F10";
        break;

        case KEY_F(11):
            key.key = "KEY_F11";
        break;

        case KEY_F(12):
            key.key = "KEY_F12";
        break;

        case '\t':
            key.key = "KEY_TAB";
        break;

        case '\n':
            key.key = "KEY_ENTER";
        break;

        case ' ':
            key.key = "KEY_SPACE";
        break;

        case 27:
            raw_input = wgetch(input_win);

            if(raw_input == ERR || raw_input == 27){
                key.key = "ESC";
            } else {
                key.key = std::string(unctrl(raw_input));
                key.alt = true;
            }
        break;

        default:
            key.key = std::string(unctrl(raw_input));
        break;
    }

    if(raw_input == ERR && key.key != "ESC") key.key = "NONE";

    if(IsCtrl(key.key)){
        key.key.erase(0,1);
        key.key[0] = tolower(key.key[0]);
        key.ctrl = true;
    }

    return key;
}

bool TESIX_Input::IsCtrl(std::string &str){
    if(str[0] == '^' && str.length() != 1) return true;
    else return false;
}
