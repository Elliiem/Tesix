#include <ncurses.h>

#include <locale>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

#include "TESIX_File.h"

#include "tree_sitter/api.h"

extern "C"{
    TSLanguage *tree_sitter_cpp();
}

int main(int argc, char** argv){
    std::string file_path = "./sample_files/test2.txt";
    TESIX_File file(file_path);
    file.Debug();

    return 0;
}