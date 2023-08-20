#pragma once

#include <ncurses.h>

#include "TESIX_File.h"
#include "TESIX_Window.h"

// Displays the file view
class TESIX_FileWindow : public TESIX_Window{
    public:
    TESIX_FileWindow(uint32_t width, uint32_t height, uint32_t x, uint32_t y);
    ~TESIX_FileWindow();
    public:
    void SetFile(TESIX_File* file);

    virtual void Action(TESIX_KeyPress key);

    void UpdateView();

    public:

    private:
    TESIX_File* file;

    TESIX_Location cur;

    uint32_t top;
    uint32_t left;

    private:
    void PrintLine(uint line, uint y);

    void SetCur(TESIX_Location loc);
    void SetCur(uint32_t line, uint32_t col);
    void MoveCur(uint32_t line, uint32_t col);
};