#pragma once

#include <cinttypes>
#include <string>

struct TESIX_Token{
    public:
    TESIX_Token(){

    }
    ~TESIX_Token(){

    }
    public:
    uint32_t start;
    uint32_t end;

    std::string token;
    public:

    private:

    private:
};