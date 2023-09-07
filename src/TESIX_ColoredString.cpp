#include "TESIX_ColoredString.h"
#include <iostream>

TESIX_ColoredString::TESIX_ColoredString(std::string str){
    this->str = str;
}

TESIX_ColoredString::TESIX_ColoredString(std::vector<std::pair<uint32_t, uint32_t>> colors, std::string str){
    SetColors(colors);
    this->str = str;
}


TESIX_ColoredString::~TESIX_ColoredString(){}

void TESIX_ColoredString::SetColors(std::vector<std::pair<uint32_t, uint32_t>> new_colors){
    int32_t last_valid_index = -1;

    uint32_t str_len = str.length();

    for(std::pair<uint32_t, uint32_t> color : new_colors){
        if(color.first >= str_len) break;
        else last_valid_index++;
    }

    colors.clear();

    for(int32_t i = -1;i < last_valid_index;i++){
        colors.push_back(new_colors[i]);
    }

    for(std::pair<uint32_t, uint32_t> color : colors){
        std::cout << color.first << "\n";
    }
}
