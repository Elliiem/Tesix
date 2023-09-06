#pragma once

#include<string>

struct TESIX_Error{
    public:
    TESIX_Error(){
        err = 0;
    }
    TESIX_Error(std::string type, int32_t err){
        this->type = type;
        this->err = err;
    }
    ~TESIX_Error(){

    }
    public:
    std::string type;
    int32_t err;

    public:
    
    private:

    private:
};