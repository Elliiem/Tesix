#pragma once

#include <iostream>
#include <fstream>

#include "TESIX_Selection.h"
#include "TESIX_BoundsError.h"

class TESIX_File{
    public:
    TESIX_File();
    TESIX_File(std::string& filename);
    ~TESIX_File();
    
    public:

    public:
    void Debug();

    void Save();
    void Load(std::string& filename);
    void Clear();

    void SetCh(TESIX_Location loc, char ch);
    void SetCh(uint32_t line, uint32_t col, char ch);
    void AddCh(TESIX_Location loc, char ch);
    void AddCh(uint32_t line, uint32_t col, char ch);
    void DelCh(TESIX_Location loc);
    void DelCh(uint32_t line, uint32_t col);

    void AddStr(TESIX_Location loc, std::string str);
    void AddStr(uint32_t line, uint32_t col, std::string str);

    void DelSel(TESIX_Selection selection);

    void AddLine(uint32_t line);
    void AddLines(uint32_t line, uint32_t count);
    void DelLine(uint32_t line);
    void DelLines(uint32_t line, uint32_t count);

    const std::string* GetFileString();
    std::string GetSel(TESIX_Selection sel);
    std::string GetLine(uint32_t line);

    uint32_t Len();
    uint32_t LineTextLen(uint32_t line);

    private:
    std::fstream file;
    std::string filename;

    std::string file_str;
    int line_count;

    private:
    TESIX_BoundsError CheckBounds(TESIX_Location loc);
    TESIX_BoundsError CheckBounds(uint32_t line, uint32_t col);
    void BoundsError(TESIX_BoundsError err, std::string from);
    void LineSafeguard();

    std::string ErrorLocationStr(TESIX_Location loc);
    std::string ErrorLocationStr(uint32_t line, uint32_t col);

    uint32_t LineLen(uint32_t line);
    uint32_t LineStart(uint32_t line);
    uint32_t LineEnd(uint32_t line);
    uint32_t Index(TESIX_Location loc);
    uint32_t Index(uint32_t line, uint32_t col);

    uint32_t CharCount(std::string str, char ch);
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