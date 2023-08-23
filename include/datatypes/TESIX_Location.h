#pragma once

#include <cinttypes>

struct TESIX_Location{
    public:
    TESIX_Location(){
        this->line = 0;
        this->col = 0;
    }
    TESIX_Location(uint32_t line, uint32_t col){
        this->line = line;
        this->col = col;
    }
    ~TESIX_Location(){}
    public:
    uint32_t line;
    uint32_t col;

    public:

    private:

    private:

};
