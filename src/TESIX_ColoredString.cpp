#include "TESIX_ColoredString.h"

TESIX_ColoredString::TESIX_ColoredString(std::string str){
    this->str = str;
}

TESIX_ColoredString::TESIX_ColoredString(std::vector<std::pair<uint32_t, uint32_t>> colors, std::string str){
    this->colors = colors;
    this->str = str;
}


TESIX_ColoredString::~TESIX_ColoredString(){}
