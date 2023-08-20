#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "TESIX_Selection.h"

class TESIX_File{
    public:
    TESIX_File();
    TESIX_File(std::string& filename);
    ~TESIX_File();
    
    public:
    
    public:
    void Save();
    void Load(std::string& filename);
    void Clear();

    void SetCh(TESIX_Location loc, char ch);
    void AddCh(TESIX_Location loc, char ch);
    void DelCh(TESIX_Location loc);

    void AddStr(TESIX_Location loc, std::string& str);
    void DelSel(TESIX_Selection selection);

    void AddLine(int line);
    void AddLines(int line, int count);
    void DelLine(int line);
    void DelLines(int line, int count);
     
    int Spacing(int line, int space);

    std::string GetLine(int line);
    std::vector<std::string> GetSel(TESIX_Selection sel);
    int Len();
    int LineLen(int line);

    private:
    std::fstream file;
    std::string filename;

    std::vector<std::string> lines;

    private:
    void CheckBounds(int line, int col);

};