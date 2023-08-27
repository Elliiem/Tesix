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
    std::string file_str;

    public:
    void Debug();

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
    int LineLen(uint32_t line);

    private:
    std::fstream file;
    std::string filename;

    //std::vector<std::string> lines;

    private:
    int32_t CheckBounds(uint32_t line, uint32_t col, bool ignore_zero_len = false);
    void BoundsError(int err, std::string from);
    uint32_t GetLineStart(uint32_t line);

};

/* 
Node Tree impl
Take a Node Tree and iterate over its Nodes line by line
    Impl idea:
    Pseudo Code
    std::option<uint32_t> NextLine()
    std::option<TS_Node> NextNode()
    

    std::option<uint32_t> line = NextLine()
    std::option<TESIX_Node> node = NextNode()

    // TESIX_Node Handles whitespace inbetween
    // It does that by storing not only its current state but also the last
    // That should make it easy to get the stuff between the two nodes
    // by getting the end of the last state and the start of the current state

    while(!line.empty()){ 
        if(node.empty()){
            NextLine()
            node = NextNode()
        }

        // Do stuff with the Node
    }
    


Be able to get the text in a Node ie from a range in the file and return it as a string

For each node generate a Struct containing the colors the Node should be and the text
    Selection implementation

Be able to get the line the Node is located
Be able to print this struct
*/