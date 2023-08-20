#include "TESIX_File.h"
#include <stdexcept>

TESIX_File::TESIX_File(){

}

TESIX_File::TESIX_File(std::string &filename) : file(filename){
    this->filename = filename;

    if(!file.is_open()) throw std::runtime_error("File is not open! << TESIX_File::TESIX_File(std::string& filename)");

    std::string line;
    while(getline(file, line)){
        lines.push_back(line);
    }
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
}


void TESIX_File::Clear(){
    lines.clear();
}


void TESIX_File::SetCh(TESIX_Location loc, char ch){
    CheckBounds(loc.line, loc.col);
    lines[loc.line][loc.col] = ch;
}


void TESIX_File::AddCh(TESIX_Location loc, char ch){
    CheckBounds(loc.line, loc.col - 1);
    if(loc.col == lines[loc.line].length()) lines[loc.line].append(std::string(1, ch));
    else lines[loc.line].insert(loc.col, std::string(1, ch));    
}


void TESIX_File::DelCh(TESIX_Location loc){
    CheckBounds(loc.line, loc.col);
    lines[loc.line].erase(loc.col, 1);
}


void TESIX_File::AddStr(TESIX_Location loc, std::string& str){
    if(loc.col != 0)CheckBounds(loc.line, loc.col - 1);
    if(loc.col == lines[loc.line].length()) lines[loc.line].append(str);
    else lines[loc.line].insert(loc.col, str);
}


void TESIX_File::DelSel(TESIX_Selection selection){
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
    CheckBounds(line, 0);
    lines.insert(lines.begin() + line, std::string());
}


void TESIX_File::AddLines(int line, int count){
    for(int i = 0;i < count;i++){
        AddLine(line);
    }
}


void TESIX_File::DelLine(int line){
    CheckBounds(line, 0);
    lines.erase(lines.begin() + line);
}


void TESIX_File::DelLines(int line, int count){
    for(int i = 0;i < count;i++){
        DelLine(line);
    }
}


int TESIX_File::Spacing(int line, int space){
    CheckBounds(line, 0);
    lines[line].resize(lines[line].length() + space, ' ');
    return lines[line].length();
}


std::string TESIX_File::GetLine(int line){
    if(line >= lines.size()) return std::string();
    else return lines[line];
}

std::vector<std::string> TESIX_File::GetSel(TESIX_Selection sel){
    std::vector<std::string> ret;

    if(sel.IsMultiline()){
        ret.push_back(lines[sel.start.line].substr(sel.start.col, lines[sel.start.line].length() - sel.start.col));
        for(int i = 1;i < sel.end.line - sel.start.line; i++){
            ret.push_back(lines[sel.start.line + i]);
        }
        ret.push_back(lines[sel.end.line].substr(0, sel.end.col + 1));
    } else {
        ret.push_back(lines[sel.start.line].substr(sel.start.col, sel.end.col - sel.start.col + 1));
    }

    return ret;
}

int TESIX_File::Len(){
    return lines.size();
}


int TESIX_File::LineLen(int line){
    if(line >= lines.size() || line < 0) return 0;
    return lines[line].length();
}


void TESIX_File::CheckBounds(int line, int col){
    if(line >= lines.size() || line < 0)
        throw std::runtime_error("Line is out of bounds! << TESIX_File::CheckBounds");
    else if (((col >= lines[line].length() || col < 0) && col != 0)) 
        throw std::runtime_error("Column is ot of bounds! << TESIX_File::CheckBounds");
}
