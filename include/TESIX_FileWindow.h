#include <ncurses.h>

#include "TESIX_File.h"

class TESIX_FileWindow{
    public:
    TESIX_FileWindow(WINDOW* win);
    ~TESIX_FileWindow();
    public:
    uint top;

    public:
    void SetFile(TESIX_File* file);

    void SetCur(TESIX_Location loc);
    TESIX_Location GetCur();

    void Update();

    private:
    WINDOW* win;
    uint width;
    uint height;
    uint x;
    uint y;

    TESIX_Location cur;

    TESIX_File* file;

    private:
    void PrintLine(uint line, uint y);

};