#include "TESIX_Editor.h"
#include <vector>
#include <string>

void TESIX_Editor::Add(std::string& str, TESIX_Location loc, TESIX_File* file){
    file->AddStr(loc, str);
}


void TESIX_Editor::Add(char ch, TESIX_Location loc, TESIX_File *file){
    file->AddCh(loc, ch);
}


void TESIX_Editor::Del(TESIX_Selection sel, TESIX_File *file){
    file->DelSel(sel);
}


void TESIX_Editor::DelCh(TESIX_Location loc, TESIX_File *file){
    file->DelCh(loc);
}


void TESIX_Editor::Move(TESIX_Selection sel, TESIX_Location loc, TESIX_File* file){
    std::vector<std::string> sel_vec = file->GetSel(sel);
    file->DelSel(sel);
    
    if(!sel.IsMultiline()){
        file->AddStr(TESIX_Location(loc.line, file->LineLen(loc.line)), sel_vec[0]);

        for(int i = 1;i < sel_vec.size(); i++){
            file->AddLine(loc.line + i);
            file->AddStr(TESIX_Location(loc.line + i, 0), sel_vec[i]);
        }
    } else {
        file->AddStr(TESIX_Location(loc.line, file->LineLen(loc.line)), sel_vec[0]);
    }
}


void TESIX_Editor::AddLine(uint32_t line, TESIX_File *file){
    file->AddLine(line);
}


void TESIX_Editor::DelLine(uint32_t line, TESIX_File *file){
    file->DelLine(line);
}
