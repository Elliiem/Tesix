#include <ncurses.h>

#include <locale>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

#include "TESIX_FileWindow.h"
#include "TESIX_Window.h"
#include "TESIX_Input.h"

extern "C" {
#include "tree_sitter/api.h"

TSLanguage *tree_sitter_cpp();

void actual_main()
{
    TSParser *parser = ts_parser_new();

    ts_parser_set_language(parser, tree_sitter_cpp());

    const char *source_code = "int main() { printf('sus'); return 0; }";

    TSTree *tree = ts_parser_parse_string(
        parser,
        nullptr,
        source_code,
        strlen(source_code)
    );

    TSNode root_node = ts_tree_root_node(tree);

    char *string = ts_node_string(root_node);
    printf("Syntax tree: %s\n", string);

    free(string);
    ts_tree_delete(tree);
    ts_parser_delete(parser);
}
} // extern "C"

int main(int argc, char** argv){
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE);

    std::string file_path = "./sample_files/test.txt";
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

    actual_main();

    return 0;
}