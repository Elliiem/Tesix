#include <ncurses.h>

#include "TESIX_FileWindow.h"
#include "TESIX_Editor.h"

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


void TESIX_FileWindow::MoveCur(uint32_t line, uint32_t col){
    cur.line += line;
    if(cur.line < 0) cur.line = 0;
    if(cur.line >= top + height){
        top += line;
        UpdateView();
    }else if(cur.line < top){
        top += line;
        UpdateView();
    } else {
        UpdateView();
    }

    cur.col += col;
    if(cur.col < 0) cur.col = 0;
    if(cur.col < left){
        left += col;
        UpdateView();
    }else if(cur.col + 6 >= left + width){
        left += col;
        UpdateView();
    } else {
        UpdateView();
    }
}


void TESIX_FileWindow::UpdateView(){
    wclear(win);

    uint i = 0;
    while (i < height){
        PrintLine(i + top, i);
        i++;
    }

    wrefresh(win);
}

bool IsChar(std::string str){
    return str.length() == 1;
}


void TESIX_FileWindow::Action(TESIX_KeyPress key){
    if(key.key == "KEY_DOWN"){
        if(cur.line < file->Len() - 1){
            if(cur.col > file->LineLen(cur.line + 1)) SetCur(TESIX_Location(cur.line + 1, file->LineLen(cur.line + 1)));
            else MoveCur(1, 0);
        }
    } else if(key.key == "KEY_UP"){
        if(cur.col > file->LineLen(cur.line - 1)) SetCur(TESIX_Location(cur.line - 1, file->LineLen(cur.line - 1)));
        else MoveCur(-1, 0);
    } else if(key.key == "KEY_LEFT"){
        if(cur.col <= 0 && cur.line != 0) SetCur(TESIX_Location(cur.line - 1, file->LineLen(cur.line - 1) + 1));
        MoveCur(0, -1);
    } else if(key.key == "KEY_RIGHT"){
        if(cur.line < file->Len() - 1){
            if(cur.col + 1 > file->LineLen(cur.line)) SetCur(TESIX_Location(cur.line + 1, 0));
            else MoveCur(0, 1);
        }
    } else if(key.key == "KEY_BACKSPACE"){
        if(cur.col == 0 && file->LineLen(cur.line) != 0 && cur.line != 0){
            uint32_t x = file->LineLen(cur.line - 1);
            TESIX_Editor::Move(TESIX_Selection(TESIX_Location(cur.line, 0), TESIX_Location(cur.line, file->LineLen(cur.line) - 1)), TESIX_Location(cur.line - 1, file->LineLen(cur.line - 1)), file);
            SetCur(TESIX_Location(cur.line - 1, x));
            UpdateView();
        } else if(cur.col == 0 && file->LineLen(cur.line) == 0){
            file->DelLine(cur.line);
            UpdateView();
        } else if(cur.col <= file->LineLen(cur.line) && cur.col != 0){
            TESIX_Editor::DelCh(TESIX_Location(cur.line, cur.col - 1), file);
            MoveCur(0, -1);
            UpdateView();
        }
    } else if(key.key == "KEY_SPACE"){
        if(cur.col <= file->LineLen(cur.line)){
            std::string space(" ");
            TESIX_Editor::Add(space, cur, file);
            MoveCur(1, 0);
            UpdateView();
        }
    } else if(key.key == "KEY_ENTER"){
        if(cur.col == 0){
            TESIX_Editor::AddLine(cur.line, file);
            SetCur(TESIX_Location(cur.line + 1, 0));
            UpdateView();
        } else if(cur.col == file->LineLen(cur.line)){
            // Handle at the end of the line
            TESIX_Editor::AddLine(cur.line + 1, file);
            SetCur(TESIX_Location(cur.line + 1, 0));
            UpdateView();
        } else {
            // Handle inside line
            // Get locations
            TESIX_Location start(cur.line, cur.col);
            TESIX_Location end(cur.line, file->LineLen(cur.line) - 1);
            TESIX_Location target(cur.line + 1, 0);

            // Make Space in the next line
            TESIX_Editor::AddLine(cur.line + 1, file);
            
            // Move the text to the new line
            TESIX_Editor::Move(TESIX_Selection(start, end), target, file);
            
            // Move the Cursor
            SetCur(cur.line + 1, 0);

            UpdateView();
        }
    } else if(key.key == "s" && key.ctrl){
        file->Save();
    } else if(IsChar(key.key)) {
        if(cur.col <= file->LineLen(cur.line)){
            TESIX_Editor::Add(key.key, cur, file);
            MoveCur(1, 0);
            UpdateView();
        }
    }

}


void TESIX_FileWindow::PrintLine(uint line, uint y){
    wmove(win, y, 0);

    std::string line_str = file->GetLine(line);

    std::string line_num = std::to_string(line);

    line_num.resize(line_num.length() + (4 - line_num.length()), ' ');
    line_num.append("| ");

    line_str.insert(0, line_num);

    line_str.erase(line_num.length(), left);

    line_str.resize(width, ' ');

    if(line == cur.line){
        wprintw(win, line_str.substr(0, (cur.col + 6) - left).c_str());
        wattron(win, A_REVERSE);
        wprintw(win, line_str.substr((cur.col + 6) - left, 1).c_str());
        wattroff(win, A_REVERSE);
        wprintw(win, line_str.substr((cur.col + 7) - left).c_str());
    } else {
        wprintw(win, line_str.c_str());
    }
}


void TESIX_FileWindow::SetCur(TESIX_Location loc){
    cur = loc;
    if(cur.line < 0) cur.line = 0;
    if(cur.col < 0) cur.col = 0;

    if(cur.line >= top + height){
        top += cur.line - height;
        UpdateView();
    }else if(cur.line < top){
        top = cur.line;
        UpdateView();
    } else {
        UpdateView();
    }


    if(cur.col < left){
        left = cur.col;
        UpdateView();
    }else if(cur.col + 6 >= left + width){
        left = cur.col - width;
        UpdateView();
    } else {
        UpdateView();
    }
}

void TESIX_FileWindow::SetCur(uint32_t line, uint32_t col){
    cur = TESIX_Location(line, col);
    if(cur.line < 0) cur.line = 0;
    if(cur.col < 0) cur.col = 0;

    if(cur.line >= top + height){
        top += cur.line - height;
        UpdateView();
    }else if(cur.line < top){
        top = cur.line;
        UpdateView();
    } else {
        UpdateView();
    }


    if(cur.col < left){
        left = cur.col;
        UpdateView();
    }else if(cur.col + 6 >= left + width){
        left = cur.col - width;
        UpdateView();
    } else {
        UpdateView();
    }
}
