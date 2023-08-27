#include "TESIX_File.h"

TESIX_File::TESIX_File(){}

TESIX_File::TESIX_File(std::string &filename){
    Load(filename);
}

TESIX_File::~TESIX_File(){
}

void TESIX_File::Debug(){
    std::cout << GetLineStart(3) << "\n";
}

void TESIX_File::Load(std::string &filename)
{
    if(file.is_open()) file.close();

    file.open(filename, std::ios::in);

    if(!file.is_open()){
        throw std::runtime_error("File is not open! << TESIX_File::Load");
    }

    file_str.clear();

    std::string line;
    while(getline(file, line)){
        file_str.append(line);
        file_str.append("\n");
    }
    
    
    this->filename = filename;
}

uint32_t TESIX_File::GetLineStart(uint32_t line){
    int index = 0;
    
    for(int i = 0;i < line;i++){
        index = file_str.find('\n', index) + 1;


    }    

    return index;
}
