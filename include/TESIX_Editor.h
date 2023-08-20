#pragma once

#include <string>

#include "TESIX_File.h"
// Edits files and directories

class TESIX_Editor{
    public:
    static void Add(std::string& str, TESIX_Location loc, TESIX_File* file);
    static void Del(TESIX_Selection sel, TESIX_File* file);
    static void DelCh(TESIX_Location loc, TESIX_File* file);
    static void Move(TESIX_Selection sel, TESIX_Location loc, TESIX_File* file);
    static void AddLine(uint32_t line, TESIX_File* file);
};