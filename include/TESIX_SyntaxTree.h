#pragma once

#include "tree_sitter/api.h"

extern "C"{
    TSLanguage* tree_sitter_cpp();
}

#include <vector>
#include <optional>
#include <string>

#include "TESIX_ColoredString.h"
#include "TESIX_File.h"

class TESIX_SyntaxTree{
    public:
    TESIX_SyntaxTree(TESIX_File* file);
    ~TESIX_SyntaxTree();
    public:
    TSNode cur; 
    std::vector<TSNode> errors;

    public:
    TSNode Last();
    TSNode Cur();

    std::optional<TSNode> Next();

    TESIX_ColoredString NextLine();

    void TEST();

    private:
    TSParser* parser;
    TSTree* tree;

    TSNode root;
    TSNode last;
    TSNode last_token;

    TESIX_File* file;

    bool is_last;
    bool is_line_start;

    const std::vector<std::string> token_types = {
        ",",
        ";",
        "(",
        ")",
        "{",
        "}",
        "[",
        "]",
        "+",
        "-",
        "*",
        "/",
        "%",
        "::",
        ":",
        "<",
        ">",
        "!",
        "&",
        "&&",
        "|",
        "||",
        "while",
        "for",
        "if",
        "else"
        "primitive_type",
        "type_identifier",
        "number_literal",
        "namespace_identifier",
        "identifier",
        "comment",
        "system_lib_string",
        "#include",
        "preproc_call",
        "extern",
        "string_literal"
    };
    
    std::vector<TESIX_ColoredString> multiline_lines;

    private:
    bool IsMultiline(TSNode node);
    bool IsLineStart(TSNode node);

    std::string GetNodeString();
    std::string GetNodeString(TSNode node);
    std::string GetNodeInbetween();

    std::optional<TSNode> NextSibling(TSNode node);

    std::optional<TSNode> NextNode();
    TSNode LastNode();

    bool IsToken(TSNode node);

    TESIX_Color GetNodeColor(TSNode node);
};