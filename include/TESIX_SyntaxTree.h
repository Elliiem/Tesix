#pragma once

#include "tree_sitter/api.h"

extern "C" {
TSLanguage* tree_sitter_cpp();
}

#include <cstring>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "TESIX_ColoredString.h"
#include "TESIX_File.h"

class TESIX_SyntaxTree {
  public:
    TESIX_SyntaxTree(TESIX_File* file);
    ~TESIX_SyntaxTree();

  public:
  public:
    TSNode Current();

    std::optional<TSNode> Next();
    std::optional<TSNode> Back();

    TESIX_ColoredString NextLine();

    // Debug
    void Debug();

  private:
    struct cmp_str {
        bool operator()(char const* a, char const* b) const {
            return std::strcmp(a, b) < 0;
        }
    };

    std::map<const char*, bool, cmp_str> token_types{{",", 1},
                                                     {";", 1},
                                                     {".", 1},
                                                     {"(", 1},
                                                     {")", 1},
                                                     {"{", 1},
                                                     {"}", 1},
                                                     {"[", 1},
                                                     {"]", 1},
                                                     {"+", 1},
                                                     {"-", 1},
                                                     {"*", 1},
                                                     {"/", 1},
                                                     {"%", 1},
                                                     {"::", 1},
                                                     {":", 1},
                                                     {"<", 1},
                                                     {">", 1},
                                                     {"<<", 1},
                                                     {">>", 1},
                                                     {"!", 1},
                                                     {"&", 1},
                                                     {"&&", 1},
                                                     {"|", 1},
                                                     {"||", 1},
                                                     {"=", 1},
                                                     {"!=", 1},
                                                     {"==", 1},
                                                     {"->", 1},
                                                     {"this", 1},
                                                     {"while", 1},
                                                     {"for", 1},
                                                     {"if", 1},
                                                     {"else", 1},
                                                     {"try", 1},
                                                     {"catch", 1},
                                                     {"primitive_type", 1},
                                                     {"type_identifier", 1},
                                                     {"number_literal", 1},
                                                     {"namespace_identifier", 1},
                                                     {"identifier", 1},
                                                     {"namespace_identifier", 1},
                                                     {"identifier", 1},
                                                     {"comment", 1},
                                                     {"system_lib_string", 1},
                                                     {"#include", 1},
                                                     {"preproc_call", 1},
                                                     {"extern", 1},
                                                     {"string_literal", 1},
                                                     {"return", 1},
                                                     {"const", 1},
                                                     {"NULL", 1},
                                                     {"true", 1},
                                                     {"false", 1},
                                                     {"~", 1},
                                                     {"throw", 1},
                                                     {"break", 1},
                                                     {"catch_clause", 0},
                                                     {"subscript_expression", 0},
                                                     {"subscript_argument_list", 0},
                                                     {"parenthesized_expression", 0},
                                                     {"abstract_function_declarator", 0},
                                                     {"break_statement", 0},
                                                     {"return_statement", 0},
                                                     {"template_type", 0},
                                                     {"destructor_name", 0},
                                                     {"template_argument_list", 0},
                                                     {"type_descriptor", 0},
                                                     {"if_statement", 0},
                                                     {"condition_clause", 0},
                                                     {"unary_expression", 0},
                                                     {"while_statement", 0},
                                                     {"binary_expression", 0},
                                                     {"template_function", 0},
                                                     {"preproc_include", 0},
                                                     {"translation_unit", 0},
                                                     {"init_declarator", 0},
                                                     {"\"", 0},
                                                     {"'", 0},
                                                     {"char_literal", 0},
                                                     {"escape_sequence", 0},
                                                     {"else_clause", 0},
                                                     {"reference_declarator", 0},
                                                     {"string_content", 0},
                                                     {"function_definition", 0},
                                                     {"try_statement", 0},
                                                     {"function_declarator", 0},
                                                     {"qualified_identifier", 0},
                                                     {"parameter_list", 0},
                                                     {"parameter_declaration", 0},
                                                     {"pointer_declarator", 0},
                                                     {"compound_statement", 0},
                                                     {"expression_statement", 0},
                                                     {"assignment_expression", 0},
                                                     {"call_expression", 0},
                                                     {"argument_list", 0},
                                                     {"field_expression", 0},
                                                     {"field_identifier", 0},
                                                     {"declaration", 0},
                                                     {"type_qualifier", 0},
                                                     {"pointer_expression", 0},
                                                     {"throw_statement", 0},
                                                     {"null", 0}};

    std::map<const char*, TESIX_Color, cmp_str> token_colors{
        {"string_literal", TESIX_Color(TESIX_COLORS_STRING)},  {"system_lib_string", TESIX_Color(TESIX_COLORS_STRING)}, {"comment", TESIX_Color(TESIX_COLORS_COMMENT)},
        {"type_identifier", TESIX_Color(TESIX_COLORS_TYPE)},   {"primitive_type", TESIX_Color(TESIX_COLORS_PRIM_TYPE)}, {"if", TESIX_Color(TESIX_COLORS_STATEMENT)},
        {"else", TESIX_Color(TESIX_COLORS_STATEMENT)},         {"while", TESIX_Color(TESIX_COLORS_STATEMENT)},          {"for", TESIX_Color(TESIX_COLORS_STATEMENT)},
        {"preproc_call", TESIX_Color(TESIX_COLORS_STATEMENT)}, {"#include", TESIX_Color(TESIX_COLORS_STATEMENT)}};

    TSParser* parser;
    TSTree* tree;

    TESIX_File* file;

    TSNode root;

    TSNode current;

    TSNode start;
    TSNode end;

    bool is_start;
    bool is_end;

    std::vector<TESIX_ColoredString> line_queue;
    std::vector<TSNode> errors;

  private:
    // Node Getters
    std::optional<TSNode> GetNext(TSNode& node);
    std::optional<TSNode> GetPrev(TSNode& node);
    std::optional<TSNode> GetNextNode(TSNode& node);
    std::optional<TSNode> GetPrevNode(TSNode& node);

    TSNode GetLineStart(TSNode& node);
    TSNode GetNextLineStart(TSNode& node);
    TSNode GetLowestLeftSideChild(TSNode& node);
    TSNode GetLowestRightSideChild(TSNode& node);

    // Node Iterators
    std::optional<TSNode> NextNode();
    std::optional<TSNode> PrevNode();

    // String Getters
    std::string GetNodeString();
    std::string GetNodeString(TSNode& node);
    std::string GetNodeInbetween(TSNode& prev, TSNode& cur);
    std::string GetNodeInbetween(TSNode& node);
    std::string GetLinePrel(TSNode& node);

    // Line Getters
    std::pair<TESIX_ColoredString, TSNode> GetLine(TSNode& node);
    TESIX_ColoredString GetMultilineFirstLine(TSNode& node);

    // Line Queue Adders and Poller
    void AddPrevEmptyLines(TSNode& node);
    void AddMultilineNodeLines(TSNode& node);
    TESIX_ColoredString PollLineQueue();

    // Helpers
    TESIX_Color GetNodeColor(TSNode node);

    bool IsMultiline(TSNode node);
    bool IsLineStart(TSNode& node);
    bool IsLineStart(TSNode& prev, TSNode& cur);
    bool IsToken(TSNode& node);
    bool HasPrevSibling(TSNode& node);
    bool HasNextSibling(TSNode& node);
    bool HasChildren(TSNode& node);

    bool Compare(TSNode& node1, TSNode& node2);
};