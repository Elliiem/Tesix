#include "TESIX_ColoredString.h"
#include <iostream>

TESIX_Color::TESIX_Color(uint32_t base_col){
    base = base_col;
    cur = base;
}

TESIX_Color::TESIX_Color(){
    base = TESIX_COLORS_NULL;
    cur = base;
}

TESIX_Color::~TESIX_Color(){}


TESIX_Color TESIX_Color::ClearMod(){
    if(base != TESIX_COLORS_NULL){
        cur = base;
    }
    return *this;
}

TESIX_Color TESIX_Color::ToError(){
    if(base != TESIX_COLORS_NULL){
        cur = base + 2;
    }
    return *this;
}

TESIX_Color TESIX_Color::ToHighlight(){
    if(base != TESIX_COLORS_NULL){
        cur = base + 1;
    }
    return *this;
}


bool TESIX_Color::operator==(TESIX_Color& other){
    return other.GetBase() == base && other.GetCurrent() == cur;
}


uint32_t TESIX_Color::GetCurrent(){
    return cur;
}

uint32_t TESIX_Color::GetBase(){
    return base;
}


TESIX_ColorIndexPair::TESIX_ColorIndexPair(TESIX_Color color, uint32_t index){
    this->color = color;
    this->index = index;
}

TESIX_ColorIndexPair::TESIX_ColorIndexPair(){
    index = 0;
}

TESIX_ColorIndexPair::~TESIX_ColorIndexPair(){}


TESIX_ColoredString::TESIX_ColoredString(std::string str){
    next_cur_index = 0;
    this->colored_string = str;
    colors.push_back(TESIX_ColorIndexPair{TESIX_Color(TESIX_COLORS_NULL), 0});
}

TESIX_ColoredString::TESIX_ColoredString(std::vector<TESIX_ColorIndexPair> colors, std::string str){
    next_cur_index = 0;
    this->colored_string = str;
    SetColors(colors);
}

TESIX_ColoredString::~TESIX_ColoredString(){}


TESIX_ColorStringPair TESIX_ColoredString::NextColorSegment(){
    if(!(next_cur_index >= colors.size())){
        uint32_t start = colors[next_cur_index].index;
        uint32_t len = 0;

        //If the index is the last color
        if(next_cur_index == colors.size() - 1) len = colored_string.length() - start;
        // ...and if its not
        else len = colors[next_cur_index + 1].index - start;

        std::string ret_str = colored_string.substr(start, len);
        
        next_cur_index++;
        
        return TESIX_ColorStringPair(ret_str, ColorAt(start));
    } else {
        return TESIX_ColorStringPair();
    }
}


void TESIX_ColoredString::SetHighlight(uint32_t start, uint32_t end){
    std::pair<uint32_t, uint32_t> interval = CreateInterval(start, end);

    for(uint32_t i = interval.first;i <= interval.second;i++){
        colors[i].color.ToHighlight();
    }
}

void TESIX_ColoredString::SetError(uint32_t start, uint32_t end){
    std::pair<uint32_t, uint32_t> interval = CreateInterval(start, end);

    for(uint32_t i = interval.first;i <= interval.second;i++){
        colors[i].color.ToError();
    }
}

void TESIX_ColoredString::ClearMod(uint32_t start, uint32_t end){
    std::pair<uint32_t, uint32_t> interval = CreateInterval(start, end);

    for(uint32_t i = interval.first;i <= interval.second;i++){
        colors[i].color.ClearMod();
    }
}


TESIX_ColoredString *TESIX_ColoredString::AddColor(TESIX_ColorIndexPair color_index_pair){
    if(colors[0].color.GetCurrent() == TESIX_COLORS_NULL){
        if(color_index_pair.index == 0){
            colors[0] = color_index_pair;
        } else {
            colors[0] = TESIX_ColorIndexPair(TESIX_Color(TESIX_COLORS_NONE), 0);
            colors.push_back(color_index_pair);
        }
    } else {
        if(color_index_pair.index < colored_string.length() && color_index_pair.index > colors.back().index){
            colors.push_back(color_index_pair);
        }
    }

    return this;
}

TESIX_ColoredString* TESIX_ColoredString::InsertColor(TESIX_ColorIndexPair color_index_pair){
    if(color_index_pair.index < colored_string.length()){
        uint32_t color_index = GetColorIndex(color_index_pair.index);
        if(colors[color_index].index == color_index_pair.index){
            
        } else {
            colors.insert(colors.begin() + color_index + 1, color_index_pair);
        }
    }
    
    return this;
}


void TESIX_ColoredString::SetColors(std::vector<TESIX_ColorIndexPair> new_colors){
    colors.clear();

    colors.push_back(TESIX_ColorIndexPair{TESIX_Color(TESIX_COLORS_NULL), 0});

    for(TESIX_ColorIndexPair color : new_colors){
        AddColor(TESIX_ColorIndexPair(color.color, color.index));
    }
}

void TESIX_ColoredString::ClearColors(){
    colors.clear();

    colors.push_back(TESIX_ColorIndexPair{TESIX_Color(TESIX_COLORS_NULL), 0});
}


uint32_t TESIX_ColoredString::LastColoredIndex(){
    return colors[colors.size() - 1].index;
}


TESIX_Color TESIX_ColoredString::ColorAt(uint32_t index){
    return colors[GetColorIndex(index)].color;
}


void TESIX_ColoredString::Simplyfy(){
    TESIX_ColorIndexPair last_color;

    for(uint32_t i = 0;i < colors.size();i++){
        if(last_color.color == colors[i].color){
            colors.erase(colors.begin() + i);
        }

        last_color = colors[i];
    }
}

void TESIX_ColoredString::Append(TESIX_ColorStringPair color_string_pair){
    if(color_string_pair.string.length() != 0){    
        uint32_t end = colored_string.length();
        if(end == 0){
            colors[0].color = color_string_pair.color;
        }
        
        colored_string.append(color_string_pair.string);
        colors.push_back(TESIX_ColorIndexPair(color_string_pair.color, end));
    }        
}

std::string TESIX_ColoredString::GetStr() {
    return colored_string;
}

void TESIX_ColoredString::PrintColors() {
    for (TESIX_ColorIndexPair color : colors) {
        std::cout << "index: " << color.index << " color: " << color.color.GetCurrent() << "\n";
    }
}

uint32_t TESIX_ColoredString::GetColorIndex(uint32_t index){
    uint32_t color_count = colors.size();
    uint32_t next_index;

    for(uint32_t i = 0;i < colors.size();i++){ 
        bool is_last = i == color_count - 1;
        if(!is_last) next_index = colors[i + 1].index;   

        if(next_index > index || is_last) return i;
    }
}


std::pair<uint32_t, uint32_t> TESIX_ColoredString::CreateInterval(uint32_t start, uint32_t end){
    uint32_t start_index = GetColorIndex(start);
    if(colors[start_index].index != start){
        colors.insert(colors.begin() + start_index + 1, TESIX_ColorIndexPair(colors[start_index].color.GetCurrent(), start));
    }

    uint32_t end_index = GetColorIndex(end);
    if(colors[end_index].index != end){
        colors.insert(colors.begin() + end_index + 1, TESIX_ColorIndexPair(colors[end_index].color.GetCurrent(), end));
        end_index++;
    }

    return std::pair<uint32_t, uint32_t>(start_index, end_index);
}

TESIX_ColorStringPair::TESIX_ColorStringPair(std::string string, TESIX_Color color){
    this->string = string;
    this->color = color;
}

TESIX_ColorStringPair::TESIX_ColorStringPair(std::string string){
    this->string = string;
    color = TESIX_Color();
}


TESIX_ColorStringPair::TESIX_ColorStringPair(){}

TESIX_ColorStringPair::~TESIX_ColorStringPair(){}
