#pragma once

#include "tree_sitter/api.h"

extern "C" {
TSLanguage* tree_sitter_cpp();
}

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
    const std::vector<std::string> token_types = {",",
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
                                                  "else",
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
                                                  "string_literal",
                                                  "return"};

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
    std::optional<TSNode> GetNext(TSNode node);
    std::optional<TSNode> GetPrev(TSNode node);
    std::optional<TSNode> GetNextNode(TSNode node);
    std::optional<TSNode> GetPrevNode(TSNode node);

    TSNode GetLineStart(TSNode& node);
    TSNode GetNextLineStart(TSNode& node);
    TSNode GetLowestLeftSideChild(TSNode& node);
    TSNode GetLowestRightSideChild(TSNode& node);

    // Node Iterators
    std::optional<TSNode> NextNode();
    std::optional<TSNode> PrevNode();

    // String Getters
    std::string GetNodeString();
    std::string GetNodeString(TSNode node);
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
    bool IsToken(TSNode node);
    bool HasPrevSibling(TSNode& node);
    bool HasNextSibling(TSNode& node);
    bool HasChildren(TSNode& node);

    bool Compare(TSNode& node1, TSNode& node2);
};