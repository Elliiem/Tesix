#include "TESIX_SyntaxTree.h"
#include <chrono>

TESIX_SyntaxTree::TESIX_SyntaxTree(TESIX_File* file) {
    parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());

    this->file = file;

    const std::string source_code = *file->GetFileString();

    tree = ts_parser_parse_string(parser, NULL, source_code.c_str(), source_code.length());
    root = ts_tree_root_node(tree);
    end = GetLowestRightSideChild(root);

    cur = ts_tree_cursor_new(root);
    Next();
    start = Current();

    is_start = true;
    is_end = false;
}

TESIX_SyntaxTree::~TESIX_SyntaxTree() {
    ts_tree_delete(tree);
    ts_parser_delete(parser);
}

TSNode TESIX_SyntaxTree::Current() {
    return ts_tree_cursor_current_node(&cur);
}

std::optional<TSNode> TESIX_SyntaxTree::Next() {
    TSNode prev_last = last_token;
    last_token = Current();
    if (!NextNode().has_value()) {
        last_token = prev_last;
        return std::optional<TSNode>();
    }

    while (!IsToken(Current())) {
        if (!NextNode().has_value()) {
            last_token = prev_last;
            return std::optional<TSNode>();
        }
    }

    return Current();
}

TESIX_ColoredString TESIX_SyntaxTree::NextLine() {
    if (line_queue.size() > 0) {
        return PollLineQueue();
    };
    std::optional<TSNode> node = Current();

    if (IsMultiline(node.value())) {
        AddMultilineNodeLines(node.value());
        Next();
        return PollLineQueue();
    }

    bool to_buffer = false;

    if (TESIX_File::CharCount(GetNodeInbetween(last_token, node.value()), '\n') > 1) {
        AddEmptyLines(last_token, node.value());
        to_buffer = true;
    }

    TESIX_ColoredString ret;

    ret.Append(GetNodePrel(last_token, node.value()));
    ret.Append(TESIX_ColorStringPair(GetNodeString(node.value()), GetNodeColor(node.value())));

    node = Next();
    if (!node.has_value()) return ret;
    while (!IsLineStart(last_token, node.value())) {
        ret.Append(GetNodeInbetween(last_token, node.value()));
        ret.Append(TESIX_ColorStringPair(GetNodeString(node.value()), GetNodeColor(node.value())));

        node = Next();
        if (!node.has_value()) return ret;
    }

    if (to_buffer) {
        line_queue.push_back(ret);
        return PollLineQueue();
    }
    return ret;
}

void TESIX_SyntaxTree::Debug() {
    for (uint32_t i = 0; i < 400; i++) {
        NextLine();
    }

    ts_tree_cursor_reset(&cur, root);
    Next();
}

bool TESIX_SyntaxTree::IsMultiline(TSNode node) {
    std::string node_string = GetNodeString(node);

    if (TESIX_File::CharCount(node_string, '\n') > 1) return true;
    else return false;
}

bool TESIX_SyntaxTree::IsLineStart(TSNode& node) {
    if (Compare(node, start)) return true;

    uint32_t start_byte = ts_node_start_byte(node) - 1;

    if (file->GetInterval(start_byte, start_byte) == "\n") return true;
    else if (TESIX_File::CharCount(GetNodeInbetween(node), '\n') > 0) return true;
    else return false;
}

bool TESIX_SyntaxTree::IsLineStart(TSNode& prev, TSNode& cur) {
    if (Compare(cur, start)) return true;

    else if (TESIX_File::CharCount(GetNodeInbetween(prev, cur), '\n') > 0) return true;
    else return false;
}

std::string TESIX_SyntaxTree::GetNodeString(TSNode& node) {
    return file->GetInterval(ts_node_start_byte(node), ts_node_end_byte(node) - 1);
}

std::string TESIX_SyntaxTree::GetNodeInbetween(TSNode& start, TSNode& end) {
    return file->GetInterval(ts_node_end_byte(start), ts_node_start_byte(end) - 1);
}

std::string TESIX_SyntaxTree::GetNodeInbetween(TSNode& node) {
    std::optional<TSNode> prev = GetPrev(node);

    if (!prev.has_value()) return std::string();

    return file->GetInterval(ts_node_end_byte(prev.value()), ts_node_start_byte(node) - 1);
}

std::string TESIX_SyntaxTree::GetNodePrel(TSNode& prev, TSNode& node) {
    std::string inbetween = GetNodeInbetween(prev, node);

    uint32_t index = inbetween.find_last_of('\n');

    if (index == -1) return "";
    return inbetween.substr(index + 1);
}

std::optional<TSNode> TESIX_SyntaxTree::GetPrev(TSNode& node) {
    std::optional<TSNode> prev = GetPrevNode(node);

    if (!prev.has_value()) return prev;

    while (!IsToken(prev.value())) {
        prev = GetPrevNode(prev.value());
        if (!prev.has_value()) return prev;
    }

    return prev;
}

std::optional<TSNode> TESIX_SyntaxTree::GetNext(TSNode& node) {
    std::optional<TSNode> next = GetNextNode(node);

    if (!next.has_value()) return next;

    while (!IsToken(next.value())) {
        next = GetNextNode(next.value());
        if (!next.has_value()) return next;
    }

    return next;
}

std::optional<TSNode> TESIX_SyntaxTree::GetPrevNode(TSNode& node) {
    if (Compare(node, root)) return std::optional<TSNode>();

    TSNode ret;

    TSNode sibling = ts_node_prev_sibling(node);

    if (!ts_node_is_null(sibling)) {
        ret = GetLowestRightSideChild(sibling);
    } else {
        ret = ts_node_parent(node);
    }

    if (ts_node_is_missing(ret)) {
        errors.push_back(ret);
        return GetPrevNode(ret);
    }

    if (ts_node_is_null(ret)) {
        return std::optional<TSNode>();
    }

    return std::optional<TSNode>(ret);
}

std::optional<TSNode> TESIX_SyntaxTree::GetNextNode(TSNode& node) {
    if (Compare(node, end)) return std::optional<TSNode>();
    TSNode ret;

    TSNode sibling = ts_node_next_sibling(node);

    if (ts_node_child_count(node) > 0) {
        ret = ts_node_child(node, 0);
    } else if (!ts_node_is_null(sibling)) {
        ret = sibling;
    } else {
        TSNode parent = ts_node_parent(node);
        TSNode parent_sibling = ts_node_next_sibling(parent);
        while (ts_node_is_null(parent_sibling)) {
            parent = ts_node_parent(parent);
            parent_sibling = ts_node_next_sibling(parent);
        }
        ret = parent_sibling;
    }

    if (ts_node_is_missing(ret)) {
        errors.push_back(ret);
        return GetNextNode(ret);
    }

    if (ts_node_is_null(ret)) {
        return std::optional<TSNode>();
    }

    return std::optional<TSNode>(ret);
}

std::optional<TSNode> TESIX_SyntaxTree::NextNode() {
    TSNode cursor = Current();

    if (ts_tree_cursor_goto_first_child(&cur)) {
        return Current();
    } else if (ts_tree_cursor_goto_next_sibling(&cur)) {
        return Current();
    } else {
        ts_tree_cursor_goto_parent(&cur);
        while (!ts_tree_cursor_goto_next_sibling(&cur)) {
            ts_tree_cursor_goto_parent(&cur);
        }
        return Current();
    }

    cursor = Current();
    if (ts_node_is_missing(cursor)) {
        errors.push_back(cursor);
        return NextNode();
    }

    return std::optional<TSNode>();
}

bool TESIX_SyntaxTree::IsToken(TSNode node) {
    // try {
    return token_types.at(ts_node_type(node));
    // } catch (std::exception& e) {
    //     throw std::runtime_error(ts_node_type(node));
    // }
}

bool TESIX_SyntaxTree::Compare(TSNode& node1, TSNode& node2) {
    if (ts_node_is_null(node1) && ts_node_is_null(node2)) return false;
    return (ts_node_start_byte(node1) == ts_node_start_byte(node2)) && (ts_node_end_byte(node1) == ts_node_end_byte(node2));
}

TSNode TESIX_SyntaxTree::GetLowestLeftSideChild(TSNode& node) {
    TSNode cur = node;

    while (ts_node_child_count(cur) > 0) {
        cur = ts_node_child(cur, 0);
    }

    return cur;
}

TSNode TESIX_SyntaxTree::GetLowestRightSideChild(TSNode& node) {
    TSNode cur = node;

    uint32_t child_count = ts_node_child_count(cur);

    while (child_count > 0) {
        cur = ts_node_child(cur, child_count - 1);
        child_count = ts_node_child_count(cur);
    }

    return cur;
}

TESIX_Color TESIX_SyntaxTree::GetNodeColor(TSNode node) {
    const char* node_type(ts_node_type(node));

    if (node_type == "identifier") {
        std::string identifier_parent_type(ts_node_type(ts_node_parent(node)));
        if (identifier_parent_type == "function_declarator") {
            return TESIX_Color(TESIX_COLORS_FUNC);
        }
        return TESIX_Color(TESIX_COLORS_VAR);
    } else {
        return token_colors[node_type];
    }
}

void TESIX_SyntaxTree::AddEmptyLines(TSNode& prev, TSNode& node) {
    std::string prel = GetNodeInbetween(prev, node);

    uint32_t index = prel.find('\n');
    uint32_t next;

    while (true) {
        next = prel.find('\n', index + 1);
        if (next == -1) break;
        line_queue.push_back(prel.substr(index + 1, next - index - 1));
        index = next;
    }
}

void TESIX_SyntaxTree::AddMultilineNodeLines(TSNode& node) {
    std::string node_string = GetNodeString(node);
    TESIX_Color color = GetNodeColor(node);

    uint32_t index = 0;
    uint32_t next;

    while (true) {
        next = node_string.find('\n', index + 1);
        if (next == -1) {
            line_queue.push_back(TESIX_ColoredString(node_string.substr(index + 1), color));
            break;
        } else {
            line_queue.push_back(TESIX_ColoredString(node_string.substr(index + 1, next - index - 1), color));
            index = next;
        }
    }
}

TESIX_ColoredString TESIX_SyntaxTree::PollLineQueue() {
    TESIX_ColoredString line = line_queue.front();

    line_queue.erase(line_queue.begin());

    return line;
}