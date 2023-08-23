#include "TESIX_File.h"
#include <stdexcept>

TESIX_File::TESIX_File(){

}

TESIX_File::TESIX_File(std::string &filename) : file(filename){
    this->filename = filename;

    if(!file.is_open()) throw std::runtime_error("File is not open! << TESIX_File::TESIX_File(std::string& filename)");

    Load(filename);
}

TESIX_File::~TESIX_File(){
    file.close();
}


void TESIX_File::Save(){
    if(!file.is_open()) throw std::runtime_error("File is not open! << TESIX_File::Save");
    file.close();
    file.open(filename, std::ios::out);

    for(std::string line : lines){
        file << line << "\n";
    }
}


void TESIX_File::Load(std::string& filename){
    if(file.is_open()) file.close();
    file.open(filename, std::ios::in);
    this->filename = filename;

    lines.clear();


    std::string line;
    while(getline(file, line)){
        lines.push_back(line);
    }

    if(lines.size() == 0) lines.push_back(std::string(""));
}


void TESIX_File::Clear(){
    lines.clear();
}


void TESIX_File::SetCh(TESIX_Location loc, char ch){
    BoundsError(CheckBounds(loc.line, loc.col), "TESIX_File::SetCh");

    lines[loc.line][loc.col] = ch;
}


void TESIX_File::AddCh(TESIX_Location loc, char ch){
    int32_t err = CheckBounds(loc.line, loc.col, true);
    uint32_t line_len = LineLen(loc.line);

    if(err == -2 && loc.col == line_len) err = 0;

    BoundsError(err, "TESIX_File::AddCh");

    if(loc.col == line_len) lines[loc.line].append(std::string(1, ch));
    else lines[loc.line].insert(loc.col, std::string(1, ch));    
}


void TESIX_File::DelCh(TESIX_Location loc){
    BoundsError(CheckBounds(loc.line, loc.col), "TESIX_File::DelCh");

    lines[loc.line].erase(loc.col, 1);
}


void TESIX_File::AddStr(TESIX_Location loc, std::string& str){
    int32_t err = CheckBounds(loc.line, loc.col, true);
    uint32_t line_len = LineLen(loc.line);

    if(err == -2 && loc.col == line_len) err = 0;

    BoundsError(err, "TESIX_File::AddStr");

    if(loc.col == line_len) lines[loc.line].append(str);
    else lines[loc.line].insert(loc.col, str);
}


void TESIX_File::DelSel(TESIX_Selection selection){
    BoundsError(CheckBounds(selection.start.line, selection.start.col, true), "TESIX_File::DelSel");
    BoundsError(CheckBounds(selection.end.line, selection.end.col, true), "TESIX_File::DelSel");
    
    if(selection.IsMultiline()){
        // Delete the part of the first line in the selection
        lines[selection.start.line].erase(selection.start.col, lines[selection.start.line].length());
        // Delete first line from lines if its empty
        if(lines[selection.start.line].empty()){
            DelLine(selection.start.line);
        }

        // Calculate how many files are inbetween the first and last
        int inbtw = selection.end.line - selection.start.line - 1;
        // Delete that amount of lines
        DelLines(selection.start.line, inbtw);

        // Delete the part of the first line in the selection
        lines[selection.end.line - inbtw - 1].erase(0, selection.end.col + 1);
        //Delete last line if its empty
        if(lines[selection.end.line].empty()){
            DelLine(selection.end.line);
        }
    } else {
        // Delete the part in the selection
        lines[selection.start.line].erase(selection.start.col, selection.end.col + 1);
        // and delete the line if the line is empty as a result
        if(lines[selection.start.line].empty()) {
            DelLine(selection.start.line);
        }
    }

}


void TESIX_File::AddLine(int line){
    uint32_t err = CheckBounds(line, 0, true);

    if(err == -1 && line == Len()) err = 0;

    BoundsError(err, "TESIX_FILE::AddLine");

    if(line == Len()) lines.push_back(std::string());
    else lines.insert(lines.begin() + line, std::string());
}


void TESIX_File::AddLines(int line, int count){
    for(int i = 0;i < count;i++){
        AddLine(line);
    }
}


void TESIX_File::DelLine(int line){
    BoundsError(CheckBounds(line, 0, true), "TESIX_File::DelLine");
    if(Len() != 1) lines.erase(lines.begin() + line);
}


void TESIX_File::DelLines(int line, int count){
    for(int i = 0;i < count;i++){
        DelLine(line);
    }
}


int TESIX_File::Spacing(int line, int space){
    BoundsError(CheckBounds(line, 0, true), "TESIX_File::Spacing");
    lines[line].resize(lines[line].length() + space, ' ');
    return lines[line].length();
}


std::string TESIX_File::GetLine(int line){
    if(line >= lines.size()) return std::string();
    else return lines[line];
}

std::vector<std::string> TESIX_File::GetSel(TESIX_Selection selection){
    std::vector<std::string> ret;

    BoundsError(CheckBounds(selection.start.line, selection.start.col, true), "TESIX_File::GetSel");
    BoundsError(CheckBounds(selection.end.line, selection.end.col, true), "TESIX_File::GetSel");

    if(selection.IsMultiline()){
        ret.push_back(lines[selection.start.line].substr(selection.start.col, lines[selection.start.line].length() - selection.start.col));
        for(int i = 1;i < selection.end.line - selection.start.line; i++){
            ret.push_back(lines[selection.start.line + i]);
        }
        ret.push_back(lines[selection.end.line].substr(0, selection.end.col + 1));
    } else {
        ret.push_back(lines[selection.start.line].substr(selection.start.col, selection.end.col - selection.start.col + 1));
    }

    return ret;
}

int TESIX_File::Len(){
    return lines.size();
}


int TESIX_File::LineLen(uint32_t line){
    if(line >= lines.size()) return 0;
    return lines[line].length();
}


int32_t TESIX_File::CheckBounds(uint32_t line, uint32_t col, bool zero_len_handler){
    if(line >= lines.size()) return -1;

    uint32_t line_len = lines[line].length();

    if(zero_len_handler){ 
        if (col >= line_len && !(line_len == 0 && col == 0)) return -2;
    } else {
        if (col >= line_len) return -2;
    }

    return 0;
}


void TESIX_File::BoundsError(int error, std::string from){
    std::string error_msg(" is out of Bounds! << " + from);

    switch (error){
    case -1:
        error_msg.insert(0, "Line");
        throw std::runtime_error(error_msg);
    break;

    case -2:
        error_msg.insert(0, "Column");
        throw std::runtime_error(error_msg);
    break;
    }


}
