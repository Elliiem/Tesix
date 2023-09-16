#pragma once

#include <ncurses.h>

#include "TESIX_KeyPress.h"

#define KEY_UP_CTRL 571
#define KEY_SUP_CTRL 572
#define KEY_SUP 337
#define KEY_LEFT_CTRL 550
#define KEY_SLEFT_CTRL 551
#define KEY_RIGHT_CTRL 565
#define KEY_SRIGHT_CTRL 566
#define KEY_DOWN_CTRL 530
#define KEY_SDOWN_CTRL 531
#define KEY_SDOWN 336
#define KEY_ESC 27

class TESIX_Input {
  public:
    TESIX_Input();
    ~TESIX_Input();

  public:
    TESIX_KeyPress GetInput();

  public:
  private:
    WINDOW* input_win;
    bool IsCtrl(std::string& str);

  private:
};