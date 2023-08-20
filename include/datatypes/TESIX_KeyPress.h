#pragma once

#include <string>

struct TESIX_KeyPress{
    public:
    TESIX_KeyPress(){
        alt = false;
        ctrl = false;
    }
    ~TESIX_KeyPress(){

    }
    public:
    bool alt;
    bool ctrl;
    std::string key;
    public:

    private:

    private:
};