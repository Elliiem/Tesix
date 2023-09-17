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

    std::map<const char*, bool, cmp_str> token_type{{",", 1},
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
                                                    {"while", 1},
                                                    {"for", 1},
                                                    {"if", 1},
                                                    {"else", 1},
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
                                                    {"return", 1}};

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
    std::string GetNodeInbetween(TSNode& node);
    std::string GetLinePrel(TSNode& node);

    // Line Getters
    TESIX_ColoredString GetLine(TSNode& node);
    TESIX_ColoredString GetMultilineFirstLine(TSNode& node);

    // Line Queue Adders and Poller
    void AddPrevEmptyLines(TSNode& node);
    void AddMultilineNodeLines(TSNode& node);
    TESIX_ColoredString PollLineQueue();

    // Helpers
    TESIX_Color GetNodeColor(TSNode node);

    bool IsMultiline(TSNode node);
    bool IsLineStart(TSNode& node);
    bool IsToken(TSNode& node);
    bool HasPrevSibling(TSNode& node);
    bool HasNextSibling(TSNode& node);
    bool HasChildren(TSNode& node);

    bool Compare(TSNode& node1, TSNode& node2);
};