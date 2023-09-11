#include "TESIX_File.h"

TESIX_File::TESIX_File(){

}

TESIX_File::TESIX_File(std::string &filename){
    Load(filename);
}

TESIX_File::~TESIX_File(){
    file.close();
}


void TESIX_File::Debug(){

}


void TESIX_File::Save(){
    file.close();
    file.open(filename, std::ios::out);
    file << file_str; 
    file.close();
    file.open(filename, std::ios::in);
}

void TESIX_File::Load(std::string &filename){
    if(file.is_open()) file.close();

    file.open(filename, std::ios::in);

    if(!file.is_open()){
        throw std::runtime_error("File is not open! << TESIX_File::Load");
    }

    file_str.clear();
    line_count = 0;

    std::string line;
    while(getline(file, line)){
        file_str.append(line);
        file_str.append("\n");
        line_count++;
    }
    
    
    this->filename = filename;
}

void TESIX_File::Clear(){
    file_str = "";
    line_count = 0;
}


void TESIX_File::SetCh(TESIX_Location loc, char ch){
    uint32_t index = Index(loc);
    
    if(index != LineEnd(loc.line)) file_str[index] = ch;
    else throw std::runtime_error("Attempted to replace newline! " + ErrorLocationStr(loc) + " << TESIX_File::SetCh");
}

void TESIX_File::SetCh(uint32_t line, uint32_t col, char ch){
    TESIX_Location loc(line, col);
    SetCh(loc, ch);
}

void TESIX_File::AddCh(TESIX_Location loc, char ch){
    uint32_t index = Index(loc);

    file_str.insert(index, std::string(1, ch));
}

void TESIX_File::AddCh(uint32_t line, uint32_t col, char ch){
    TESIX_Location loc(line, col);
    AddCh(loc, ch);
}

void TESIX_File::DelCh(TESIX_Location loc){
    uint32_t index = Index(loc);

    if(index != LineEnd(loc.line)) file_str.erase(index, 1);
    else throw std::runtime_error("Attemted to erase newline! " + ErrorLocationStr(loc) + " << TESIX_File::DelCh");
}

void TESIX_File::DelCh(uint32_t line, uint32_t col){
    TESIX_Location loc(line, col);

    DelCh(loc);
}


void TESIX_File::AddStr(TESIX_Location loc, std::string str){
    uint32_t index = Index(loc);

    uint32_t newline_count = CharCount(str, '\n');
    line_count += newline_count;

    file_str.insert(index, str);
}

void TESIX_File::AddStr(uint32_t line, uint32_t col, std::string str){
    TESIX_Location loc(line, col);
    AddStr(loc, str);
}


void TESIX_File::DelSel(TESIX_Selection sel){
    BoundsError(CheckBounds(sel.start), "TESIX_File::DelSel");
    BoundsError(CheckBounds(sel.end), "TESIX_File::DelSel");

    uint32_t start = Index(sel.start);
    uint32_t end = Index(sel.end);

    uint32_t len = (end - start) + 1;

    std::string str = GetSel(sel);
    
    uint32_t newline_count = CharCount(str, '\n');
    line_count -= newline_count;

    file_str.erase(start, len);

    LineSafeguard();
}


void TESIX_File::AddLine(uint32_t line){
    if(line == Len()){
        file_str.append("\n");
        line_count++;
    } else {
        uint32_t index = LineStart(line);
    
        file_str.insert(index, "\n");
        line_count++;
    }
}

void TESIX_File::AddLines(uint32_t line, uint32_t count){
    for(uint32_t i = 0;i < count;i++) AddLine(line);
}

void TESIX_File::DelLine(uint32_t line){
    uint32_t start = LineStart(line);
    uint32_t len = LineLen(line);

    file_str.erase(start, len);
    line_count--;

    LineSafeguard();
}

void TESIX_File::DelLines(uint32_t line, uint32_t count){
    uint32_t start = LineStart(line);
    uint32_t end = LineEnd(line + (count - 1));

    uint32_t len = (end - start) + 1;

    file_str.erase(start, len);
    line_count -= count;

    LineSafeguard();
}

const std::string *TESIX_File::GetFileString(){
    return &file_str;
}

std::string TESIX_File::GetSel(TESIX_Selection sel){
    BoundsError(CheckBounds(sel.start), "TESIX_File::GetSel");
    BoundsError(CheckBounds(sel.end), "TESIX_File::GetSel");

    uint32_t start = Index(sel.start);
    uint32_t end = Index(sel.end);

    uint32_t len = (end - start) + 1;

    return file_str.substr(start, len);
}

std::string TESIX_File::GetLine(uint32_t line){
    uint32_t start = LineStart(line);
    uint32_t len = LineLen(line);
    
    return file_str.substr(start, len - 1);
}

std::string TESIX_File::GetInterval(uint32_t start, uint32_t end){
    return file_str.substr(start, end - start + 1);
}

uint32_t TESIX_File::Len(){
    return line_count;
}

uint32_t TESIX_File::LineTextLen(uint32_t line){
    if(line >= line_count){
        return 0;
    }
    
    uint32_t len = LineLen(line) - 1;

    return len;
}

uint32_t TESIX_File::LineLen(uint32_t line){
    if(line >= line_count){
        throw std::runtime_error("Line is out of bounds! " + ErrorLocationStr(TESIX_Location(line, 0)) + " << TESIX_File::LineLen");
    }

    uint32_t start = LineStart(line);
    uint32_t end = LineEnd(line);

    uint32_t len = (end - start) + 1;

    return len;
}


TESIX_BoundsError TESIX_File::CheckBounds(TESIX_Location loc){
    if(loc.line >= line_count) return TESIX_BoundsError(loc, -2);
    else if(loc.col >= LineLen(loc.line)) return TESIX_BoundsError(loc, -1);
    else return TESIX_BoundsError();
}

TESIX_BoundsError TESIX_File::CheckBounds(uint32_t line, uint32_t col){
    TESIX_Location loc(line, col);
    return CheckBounds(loc);
}

void TESIX_File::BoundsError(TESIX_BoundsError err, std::string from){
    if(err.err == -1){
        throw std::runtime_error("Column is out of bounds! " + ErrorLocationStr(err.loc) + " << " + from);
    }
    else if(err.err == -2){
        throw std::runtime_error("Line is out of bounds! " + ErrorLocationStr(err.loc) + " << " + from);
    }
}

void TESIX_File::LineSafeguard(){
    if(file_str[file_str.length() - 1] != '\n'){
        file_str.append("\n");
        line_count++;
    }
}


std::string TESIX_File::ErrorLocationStr(TESIX_Location loc){
    return "(line: " + std::to_string(loc.line) + " col: " + std::to_string(loc.col) + ")";
}

std::string TESIX_File::ErrorLocationStr(uint32_t line, uint32_t col){
    TESIX_Location loc(line, col);

    return ErrorLocationStr(loc);
}


uint32_t TESIX_File::LineStart(uint32_t line){
    if(line >= line_count) BoundsError(TESIX_BoundsError(TESIX_Location(line, 0), -2), "TESIX_File::LineStart");
    
    int index = 0;
    
    for(uint32_t i = 0;i < line;i++){
        index = file_str.find('\n', index) + 1;
    }    

    return index;
}


uint32_t TESIX_File::LineEnd(uint32_t line){
    if(line >= line_count) BoundsError(TESIX_BoundsError(TESIX_Location(line, 0), -1), "TESIX_File::LineEnd");
    
    int index = 0;
    index = file_str.find('\n', index);
    
    for(uint32_t i = 0;i < line;i++){
        index = file_str.find('\n', index) + 1;
    }

    index = file_str.find('\n', index);

    return index;
}


uint32_t TESIX_File::Index(TESIX_Location loc){
    BoundsError(CheckBounds(loc), "TESIX_File::Index");

    return LineStart(loc.line) + loc.col;
}

uint32_t TESIX_File::Index(uint32_t line, uint32_t col){
    TESIX_Location loc(line, col);
    return Index(loc);
}


uint32_t TESIX_File::CharCount(std::string str, char ch){
    int index = 0;
    uint32_t count = 0;

    while(true){
        index = str.find(ch, index);
        if(index == -1) break;
        else{
            count++;
            index++;
        }
    }
    
    return count;
}
