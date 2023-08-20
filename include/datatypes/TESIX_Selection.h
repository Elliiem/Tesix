#pragma once

#include "TESIX_Location.h"

struct TESIX_Selection{
    public:
    TESIX_Selection(){}
    TESIX_Selection(TESIX_Location start, TESIX_Location end){
        this->start = start;
        this->end = end;
    }
    ~TESIX_Selection(){}
    public:
    TESIX_Location start;
    TESIX_Location end;

    public:
    bool IsMultiline();

    private:

    private:

};