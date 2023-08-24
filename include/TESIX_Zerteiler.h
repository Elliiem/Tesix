#pragma once

#include <vector>

#include "TESIX_Token.h"
#include "TESIX_File.h"

class TESIX_Parsed_File : public TESIX_File{
    public:
    TESIX_Parsed_File();
    ~TESIX_Parsed_File();
    public:
    std::vector<std::vector<TESIX_Token>> tokenized_lines;

    // Tree parsed;


    TESIX_File* file;

    public:

    private:

    private:

};