#include "TESIX_SyntaxTree.h"

TESIX_SyntaxTree::TESIX_SyntaxTree(TESIX_File* file){
    parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());

    this->file = file;

    const std::string source_code = *file->GetFileString();

    tree = ts_parser_parse_string(
        parser,
        NULL,
        source_code.c_str(),
        source_code.length()
    );

    root = ts_tree_root_node(tree);
    current = root;

    is_start = true;
    if(HasChildren(root)){
        start = ts_node_child(root, 0);
    } else {
        start = root;
    }

    end = GetLowestRightSideChild(root);
    is_end = false;
}

TESIX_SyntaxTree::~TESIX_SyntaxTree(){
    ts_tree_delete(tree);
    ts_parser_delete(parser);
}

TSNode TESIX_SyntaxTree::Current(){
    return current;
}

std::optional<TSNode> TESIX_SyntaxTree::Next(){
    if(!NextNode().has_value()) return std::optional<TSNode>();
    
    while(!IsToken(current)){
        if(!NextNode().has_value()) return std::optional<TSNode>();
    }

    return std::optional<TSNode>(current);
}

std::optional<TSNode> TESIX_SyntaxTree::Back(){
    if(!PrevNode().has_value()) return std::optional<TSNode>();
    
    while(!IsToken(current)){
        if(!PrevNode().has_value()) return std::optional<TSNode>();
    }

    return std::optional<TSNode>(current);
}

TESIX_ColoredString TESIX_SyntaxTree::NextLine(){
    
    
    



    return TESIX_ColoredString("");
}

void TESIX_SyntaxTree::Debug(){
    Next();

    TSNode line_start = GetLineStart(current);

    std::cout << GetNodeString(line_start) << "|\n";
    std::cout << GetNodeString(GetNext(line_start).value()) << "|\n";
}

bool TESIX_SyntaxTree::IsMultiline(TSNode node){
    std::string node_string = GetNodeString(node);

    uint32_t index = node_string.find('\n', 0);
    if(index == -1) return false;

    index = node_string.find('\n', index + 1);
    if(index == -1) return false;

    return true;
}

bool TESIX_SyntaxTree::IsLineStart(TSNode& node){
    std::string inbetween = GetNodeInbetween(node);

    if(TESIX_File::CharCount(inbetween, '\n') > 0) return true;
    else return false;
}

std::string TESIX_SyntaxTree::GetNodeString(){
    return file->GetInterval(ts_node_start_byte(current), ts_node_end_byte(current) - 1);
}

std::string TESIX_SyntaxTree::GetNodeString(TSNode node){
    return file->GetInterval(ts_node_start_byte(node), ts_node_end_byte(node) - 1);
}

std::string TESIX_SyntaxTree::GetNodeInbetween(TSNode& node){
    std::optional<TSNode> prev = GetPrev(node);

    if(!prev.has_value()) return std::string();

    return file->GetInterval(ts_node_end_byte(prev.value()), ts_node_start_byte(node) - 1);
}

std::optional<TSNode> TESIX_SyntaxTree::GetPrev(TSNode node){
    std::optional<TSNode> prev = GetPrevNode(current);

    if (!prev.has_value()) return prev;

    while (!IsToken(prev.value())) {
        prev = GetPrevNode(prev.value());
        if(!prev.has_value()) return prev;
    }

    if (is_start) is_start = false;
    if (Compare(prev.value(), end)) is_end = true;

    return prev;
}

std::optional<TSNode> TESIX_SyntaxTree::GetNext(TSNode node) {
    std::optional<TSNode> next = GetNextNode(current);

    if (!next.has_value()) return next;

    while (!IsToken(next.value())) {
        next = GetNextNode(next.value());
        if (!next.has_value()) return next;
    }

    if (is_start) is_start = false;
    if (Compare(next.value(), end)) is_end = true;

    return next;
}

std::optional<TSNode> TESIX_SyntaxTree::GetPrevNode(TSNode node) {
    if(is_start) return std::optional<TSNode>();
    if(Compare(node, root)) return std::optional<TSNode>();

    TSNode ret;

    if(HasPrevSibling(node)){
        TSNode sibling = ts_node_prev_sibling(node);
        ret = GetLowestRightSideChild(sibling);
    } else {
        ret = ts_node_parent(node);
    }

    if(ts_node_is_missing(ret)){
        errors.push_back(ret);
        return GetPrevNode(ret);
    }

    if(ts_node_is_null(ret)){
        return std::optional<TSNode>();
    }

    return std::optional<TSNode>(ret);
}

std::optional<TSNode> TESIX_SyntaxTree::GetNextNode(TSNode node){
    if(is_end) return std::optional<TSNode>();
    if(Compare(node, end)) return std::optional<TSNode>();

    TSNode ret;

    if(HasChildren(node)){
        ret = ts_node_child(node, 0);
    } else if(HasNextSibling(node)){
        ret = ts_node_next_sibling(node);
    } else {
        TSNode parent = ts_node_parent(node);
        while(!HasNextSibling(parent)){
            parent = ts_node_parent(parent);
        }
        ret = ts_node_next_sibling(parent);
    }

    if(ts_node_is_missing(ret)){
        errors.push_back(ret);
        return GetNextNode(ret);
    }

    if(ts_node_is_null(ret)){
        return std::optional<TSNode>();
    }

    return std::optional<TSNode>(ret);
}

std::optional<TSNode> TESIX_SyntaxTree::NextNode() {
    std::optional<TSNode> next = GetNextNode(current);

    if(!next.has_value()) return next;
    if(is_start) is_start = false;
    if(Compare(next.value(), end)) is_end = true;

    current = next.value();

    return std::optional<TSNode>(next);
}

std::optional<TSNode> TESIX_SyntaxTree::PrevNode() {
    std::optional<TSNode> prev = GetPrevNode(current);

    if(!prev.has_value()) return prev;
    if(is_end) is_end = false;
    if(Compare(prev.value(), start)) is_start = true;

    current = prev.value();

    return std::optional<TSNode>(prev);
}


bool TESIX_SyntaxTree::IsToken(TSNode node){
    std::string type(ts_node_type(node));

    bool is_token = false;
    
    for(std::string token_type : token_types){
        if(type == token_type) is_token = true;
    }
    
    return is_token;
}

bool TESIX_SyntaxTree::HasPrevSibling(TSNode& node) {
    return !ts_node_is_null(ts_node_prev_sibling(node));
}

bool TESIX_SyntaxTree::HasNextSibling(TSNode& node){
    return !ts_node_is_null(ts_node_next_sibling(node));
}

bool TESIX_SyntaxTree::HasChildren(TSNode& node) {
    if(ts_node_child_count(node) != 0) return true;
    else return false;
}

bool TESIX_SyntaxTree::Compare(TSNode& node1, TSNode& node2) {
    if(ts_node_is_null(node1) && ts_node_is_null(node2)) return false;
    return (ts_node_start_byte(node1) == ts_node_start_byte(node2)) && (ts_node_end_byte(node1) == ts_node_end_byte(node2));
}

TSNode TESIX_SyntaxTree::GetLowestLeftSideChild(TSNode& node){
    TSNode cur = node;
    
    while(HasChildren(cur)){
        cur = ts_node_child(cur, 0);
    }
    
    return cur;
}

TSNode TESIX_SyntaxTree::GetLowestRightSideChild(TSNode& node){
    TSNode cur = node;

    while(HasChildren(cur)){
        cur = ts_node_child(cur, ts_node_child_count(cur) - 1);
    }

    return cur;
}

TESIX_Color TESIX_SyntaxTree::GetNodeColor(TSNode node){
    std::string node_type(ts_node_type(node));

    // Var
    if(node_type == "identifier"){
        std::string identifier_parent_type(ts_node_type(ts_node_parent(node)));
        if(identifier_parent_type == "function_declarator"){
            return TESIX_Color(TESIX_COLORS_FUNC);
        }
        return TESIX_Color(TESIX_COLORS_VAR);
    }
    // String
    else if(node_type == "string_literal"){
        return TESIX_Color(TESIX_COLORS_STRING);
    } else if(node_type == "system_lib_string"){
        return TESIX_Color(TESIX_COLORS_STRING);
    }
    // Comment
    else if(node_type == "comment"){
        return TESIX_Color(TESIX_COLORS_COMMENT);
    }
    // Type
    else if(node_type == "type_identifier"){
        return TESIX_Color(TESIX_COLORS_TYPE);
    }
    // Prim types
    else if(node_type == "primitive_type"){
        return TESIX_Color(TESIX_COLORS_PRIM_TYPE);
    }
    // Statements
    else if(node_type == "if"){
        return TESIX_Color(TESIX_COLORS_STATEMENT);
    } else if(node_type == "else"){
        return TESIX_Color(TESIX_COLORS_STATEMENT);
    } else if(node_type == "while"){
        return TESIX_Color(TESIX_COLORS_STATEMENT);
    } else if(node_type == "for"){
        return TESIX_Color(TESIX_COLORS_STATEMENT);
    } else if(node_type == "preproc_call"){
        return TESIX_Color(TESIX_COLORS_STATEMENT);
    } else if(node_type == "#include"){
        return TESIX_Color(TESIX_COLORS_STATEMENT);
    }

    return TESIX_Color();
}

TESIX_ColoredString TESIX_SyntaxTree::GetLine(TSNode& node){








    return TESIX_ColoredString("");
}

TESIX_ColoredString TESIX_SyntaxTree::AddPrevEmptyLines(TSNode& node){
    
    return TESIX_ColoredString("");
}

TESIX_ColoredString TESIX_SyntaxTree::AddMultilineNodeLines(TSNode& node) {

    return TESIX_ColoredString("");
}

TSNode TESIX_SyntaxTree::GetLineStart(TSNode& node){
    if(IsLineStart(node)) return node;
    
    std::optional<TSNode> search = GetPrev(node);

    if(!search.has_value()){
        if(IsToken(node)) return node;
        else return GetNext(node).value();
    }

    while(!IsLineStart(search.value())){
        search = GetPrev(search.value());

        if(!search.has_value()){
            if(IsToken(node)) return node;
            else return GetNext(node).value();
        }
    }

    return search.value();
}

TESIX_ColoredString TESIX_SyntaxTree::PollLineQueue(){
    TESIX_ColoredString line = line_queue.front();

    line_queue.erase(line_queue.begin());

    return line;
}
