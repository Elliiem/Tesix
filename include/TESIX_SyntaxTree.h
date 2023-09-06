#pragma once

#include "tree_sitter/api.h"

extern "C"{
    TSLanguage *tree_sitter_cpp();
}

#include <vector>
#include <optional>
#include <string>

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
    "primitive_type",
    "type_identifier",
    "number_literal",
    "namespace_identifier",
    "identifier",
    "comment"
};

class TESIX_SyntaxTree{
    public:
    TESIX_SyntaxTree(TSNode root_node);
    ~TESIX_SyntaxTree();
    public:
    TSNode cur; 
    std::vector<TSNode> errors;

    public:
    TSNode* Next();

    TSNode* Last();

    TSNode* NextToken();

    private:
    TSNode last;
    
    private:
    std::optional<TSNode> NextSibling(TSNode node);

    bool IsToken(TSNode node);
};