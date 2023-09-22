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

    current = GetNext(root).value();

    is_start = true;
    is_end = false;

    start = current;
}

TESIX_SyntaxTree::~TESIX_SyntaxTree() {
    ts_tree_delete(tree);
    ts_parser_delete(parser);
}

TSNode TESIX_SyntaxTree::Current() {
    return current;
}

std::optional<TSNode> TESIX_SyntaxTree::Next() {
    if (!NextNode().has_value()) return std::optional<TSNode>();

    while (!IsToken(current)) {
        if (!NextNode().has_value()) return std::optional<TSNode>();
    }

    return std::optional<TSNode>(current);
}

std::optional<TSNode> TESIX_SyntaxTree::Back() {
    if (!PrevNode().has_value()) return std::optional<TSNode>();

    while (!IsToken(current)) {
        if (!PrevNode().has_value()) return std::optional<TSNode>();
    }

    return std::optional<TSNode>(current);
}

TESIX_ColoredString TESIX_SyntaxTree::NextLine() {
    if (line_queue.size() != 0) {
        TESIX_ColoredString str = PollLineQueue();
        return str;
    }

    if (IsMultiline(current)) {
        AddMultilineNodeLines(current);
        NextNode();
        return PollLineQueue();
    }

    if (TESIX_File::CharCount(GetNodeInbetween(current), '\n') > 1) {
        AddPrevEmptyLines(current);
        std::pair<TESIX_ColoredString, TSNode> line = GetLine(current);
        line_queue.push_back(line.first);
        if (Compare(current, end)) is_end = true;
        current = line.second;
        return PollLineQueue();
    }

    std::pair<TESIX_ColoredString, TSNode> line = GetLine(current);
    if (Compare(current, end)) is_end = true;
    current = line.second;
    return line.first;
}

void TESIX_SyntaxTree::Debug() {
    /*while (!is_end) {
        // std::cout << NextLine().GetStr() << "|\n";
        NextLine();
    }
    current = start;
    is_start = true;
    is_end = false;*/

    uint32_t count = 100000;
    uint32_t sum_cursor = 0;
    uint32_t sum_func = 0;

    TSTreeCursor cursor = ts_tree_cursor_new(current);
    for (uint32_t i = 0; i < count; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        ts_tree_cursor_goto_next_sibling(&cursor);
        ts_tree_cursor_reset(&cursor, current);
        auto end = std::chrono::high_resolution_clock::now();
        sum_cursor += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    for (uint32_t i = 0; i < count; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        ts_node_next_sibling(current);
        auto end = std::chrono::high_resolution_clock::now();
        sum_func += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    std::cout << "cursor: " << sum_cursor / count << "\n";
    std::cout << "func: " << sum_func / count << "\n";
    std::cout << "ratio: " << (float)sum_func / (float)sum_cursor << "\n";
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

std::string TESIX_SyntaxTree::GetNodeString() {
    return file->GetInterval(ts_node_start_byte(current), ts_node_end_byte(current) - 1);
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

std::string TESIX_SyntaxTree::GetLinePrel(TSNode& line_start) {
    std::string inbetween = GetNodeInbetween(line_start);

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

    if (HasChildren(node)) {
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
    std::optional<TSNode> next = GetNextNode(current);

    if (!next.has_value()) return next;
    if (Compare(next.value(), end)) is_end = true;

    current = next.value();
    is_start = false;

    return std::optional<TSNode>(next);
}

std::optional<TSNode> TESIX_SyntaxTree::PrevNode() {
    std::optional<TSNode> prev = GetPrevNode(current);

    if (!prev.has_value()) return prev;
    if (Compare(prev.value(), root)) is_start = true;

    current = prev.value();
    is_end = false;

    return std::optional<TSNode>(prev);
}

bool TESIX_SyntaxTree::IsToken(TSNode& node) {
    try {
        return token_types.at(ts_node_type(node));
    } catch (std::exception& e) {
        throw std::runtime_error(ts_node_type(node));
    }
}

bool TESIX_SyntaxTree::HasPrevSibling(TSNode& node) {
    return !ts_node_is_null(ts_node_prev_sibling(node));
}

bool TESIX_SyntaxTree::HasNextSibling(TSNode& node) {
    return !ts_node_is_null(ts_node_next_sibling(node));
}

bool TESIX_SyntaxTree::HasChildren(TSNode& node) {
    if (ts_node_child_count(node) != 0) return true;
    else return false;
}

bool TESIX_SyntaxTree::Compare(TSNode& node1, TSNode& node2) {
    if (ts_node_is_null(node1) && ts_node_is_null(node2)) return false;
    return (ts_node_start_byte(node1) == ts_node_start_byte(node2)) && (ts_node_end_byte(node1) == ts_node_end_byte(node2));
}

TSNode TESIX_SyntaxTree::GetLowestLeftSideChild(TSNode& node) {
    TSNode cur = node;

    while (HasChildren(cur)) {
        cur = ts_node_child(cur, 0);
    }

    return cur;
}

TSNode TESIX_SyntaxTree::GetLowestRightSideChild(TSNode& node) {
    TSNode cur = node;

    while (HasChildren(cur)) {
        cur = ts_node_child(cur, ts_node_child_count(cur) - 1);
    }

    return cur;
}

TESIX_ColoredString TESIX_SyntaxTree::GetMultilineFirstLine(TSNode& node) {
    std::string node_string = GetNodeString(node);

    return TESIX_ColoredString(node_string.substr(0, node_string.find('\n')), GetNodeColor(node));
}

TSNode TESIX_SyntaxTree::GetNextLineStart(TSNode& node) {
    std::optional<TSNode> search = GetNext(node);
    if (!search.has_value()) {
        return node;
    }

    while (!IsLineStart(search.value())) {
        search = GetNext(search.value());
        if (!search.has_value()) return node;
    }

    return search.value();
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

std::pair<TESIX_ColoredString, TSNode> TESIX_SyntaxTree::GetLine(TSNode& node) {
    std::optional<TSNode> cur = node;
    TSNode last_cur;

    if (IsMultiline(cur.value())) {
        return std::pair<TESIX_ColoredString, TSNode>(GetMultilineFirstLine(node), GetNext(cur.value()).value());
    }

    TESIX_ColoredString ret;

    ret.Append(GetLinePrel(cur.value()));
    ret.Append(TESIX_ColorStringPair(GetNodeString(cur.value()), GetNodeColor(cur.value())));

    last_cur = cur.value();
    cur = GetNext(cur.value());
    if (!cur.has_value()) return std::pair<TESIX_ColoredString, TSNode>(ret, last_cur);

    while (!IsLineStart(last_cur, cur.value())) {
        ret.Append(GetNodeInbetween(last_cur, cur.value()));
        ret.Append(TESIX_ColorStringPair(GetNodeString(cur.value()), GetNodeColor(cur.value())));

        last_cur = cur.value();
        cur = GetNext(cur.value());
        if (!cur.has_value()) return std::pair<TESIX_ColoredString, TSNode>(ret, last_cur);
    }

    return std::pair<TESIX_ColoredString, TSNode>(ret, cur.value());
}

void TESIX_SyntaxTree::AddPrevEmptyLines(TSNode& node) {
    std::string prel = GetNodeInbetween(node);

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

    line_queue.push_back(GetMultilineFirstLine(node));

    uint32_t index = node_string.find('\n');
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

TSNode TESIX_SyntaxTree::GetLineStart(TSNode& node) {
    if (IsLineStart(node)) return node;

    std::optional<TSNode> search = GetPrev(node);

    if (!search.has_value()) {
        if (IsToken(node)) return node;
        else return GetNext(node).value();
    }

    while (!IsLineStart(search.value())) {
        search = GetPrev(search.value());

        if (!search.has_value()) {
            if (IsToken(node)) return node;
            else return GetNext(node).value();
        }
    }

    return search.value();
}

TESIX_ColoredString TESIX_SyntaxTree::PollLineQueue() {
    TESIX_ColoredString line = line_queue.front();

    line_queue.erase(line_queue.begin());

    return line;
}
