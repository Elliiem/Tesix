#pragma once

#include "TESIX_Error.h"
#include "TESIX_Location.h"

struct TESIX_BoundsError : public TESIX_Error{
    public:
    TESIX_BoundsError(): TESIX_Error(){

    }
    TESIX_BoundsError(TESIX_Location loc, int32_t err) : TESIX_Error(std::string("BoundsError"), err){
        this->loc = loc;
    }
    ~TESIX_BoundsError(){

    }
    public:
    TESIX_Location loc;

    public:

    private:

    private:
};