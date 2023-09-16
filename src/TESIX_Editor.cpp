#include "TESIX_Editor.h"
#include <string>
#include <vector>

void TESIX_Editor::Add(std::string& str, TESIX_Location loc, TESIX_File* file) {
    file->AddStr(loc, str);
}

void TESIX_Editor::Add(char ch, TESIX_Location loc, TESIX_File* file) {
    file->AddCh(loc, ch);
}

void TESIX_Editor::Del(TESIX_Selection sel, TESIX_File* file) {
    file->DelSel(sel);
}

void TESIX_Editor::DelCh(TESIX_Location loc, TESIX_File* file) {
    file->DelCh(loc);
}

void TESIX_Editor::Move(TESIX_Selection sel, TESIX_Location loc, TESIX_File* file) {
    std::string line = file->GetSel(sel);

    file->DelSel(sel);

    file->AddStr(loc.line, file->LineTextLen(loc.line), line);
}

void TESIX_Editor::AddLine(uint32_t line, TESIX_File* file) {
    file->AddLine(line);
}

void TESIX_Editor::DelLine(uint32_t line, TESIX_File* file) {
    file->DelLine(line);
}
