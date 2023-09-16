#pragma once

#include <ncurses.h>

#include "TESIX_KeyPress.h"

class TESIX_Window {
  public:
    TESIX_Window(uint32_t width, uint32_t height, uint32_t x, uint32_t y);
    ~TESIX_Window();

  public:
    WINDOW* win;

  public:
    virtual void Action(TESIX_KeyPress key);

    void Refresh();

  private:
  private:
  protected:
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;

  protected:
};