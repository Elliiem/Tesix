#include <ncurses.h>

#include <locale>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <optional>

#include "TESIX_File.h"
#include "TESIX_FileWindow.h"
#include "TESIX_Input.h"
#include "TESIX_SyntaxTree.h"
#include "TESIX_ColoredString.h"

#include "tree_sitter/api.h"

extern "C"{
    TSLanguage *tree_sitter_cpp();
}


int main(int argc, char** argv){
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE);

    std::string file_path = "/home/elliem/Dev/Programs/1st-Party/cpp/Tesix/main.cpp";
    TESIX_File file(file_path);

    TESIX_FileWindow window(200, 50, 0, 0);
    window.SetFile(&file);
    window.UpdateView();

    TESIX_Input input;
    TESIX_KeyPress key;

    while(key.key != "q"){
        key = input.GetInput();
        window.Action(key);
    }

    endwin();

    // TS Shizz ------------------------------------

    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());

    const std::string source_code = *file.GetFileString();

    TSTree *tree = ts_parser_parse_string(
        parser,
        NULL,
        source_code.c_str(),
        source_code.length()
    );

    TSNode root_node = ts_tree_root_node(tree);

    TESIX_SyntaxTree foo(root_node);

    // Output
    /*TSNode* node = foo.Next();
    while(node != nullptr){
        std::cout << "\033[1;34mnode:\033[0m " << source_code.substr(ts_node_start_byte(*node), ts_node_end_byte(*node) - ts_node_start_byte(*node)) << " \033[1;31mtype:\033[0m " << ts_node_type(*node) << "\n";
        node = foo.Next();
    }

    std::cout << "\n";
    for(TSNode node : foo.errors){
        std::cout << ts_node_type(node) << "\n";
    }*/

    std::vector<std::pair<uint32_t, uint32_t>> vec = {std::pair<uint32_t, uint32_t>{0,0}, std::pair<uint32_t, uint32_t>{4,1}};
    TESIX_ColoredString str(vec, "STRING");



    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
