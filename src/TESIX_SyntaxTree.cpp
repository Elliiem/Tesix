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
    cur = root;
    last_token = cur;
    last = cur;
    Next();
}

TESIX_SyntaxTree::~TESIX_SyntaxTree(){
    ts_tree_delete(tree);
    ts_parser_delete(parser);
}

std::optional<TSNode> TESIX_SyntaxTree::NextNode(){
    last = cur;

    if(ts_node_child_count(cur) != 0){
        while(ts_node_child_count(cur) != 0){
            cur = ts_node_child(cur, 0);
        }
    } else {
        std::optional<TSNode> next = NextSibling(cur);
            
        if(!next.has_value() || ts_node_is_missing(next.value())){
            is_last = true;
            return std::optional<TSNode>();
        }
        cur = next.value();
    }

    return cur;
}

TSNode TESIX_SyntaxTree::LastNode(){
    std::swap(cur, last);
    return cur;
}

TSNode TESIX_SyntaxTree::Last(){
    std::swap(cur, last_token);
    return cur;
}

TSNode TESIX_SyntaxTree::Cur(){
    return cur;
}

std::optional<TSNode> TESIX_SyntaxTree::Next(){
    if(IsToken(cur)) last_token = cur;

    if(!NextNode().has_value()) return std::optional<TSNode>();
    
    while(!IsToken(cur)){
        if(!NextNode().has_value()) return std::optional<TSNode>();
    }

    return std::optional<TSNode>(cur);
}

TESIX_ColoredString TESIX_SyntaxTree::NextLine(){
    TESIX_ColoredString ret_str("");

    bool is_first_in_line = true;

    while(true){
        if(!is_last){
            if(!IsLineStart(cur) || is_first_in_line){
                if(is_first_in_line) is_first_in_line = false;
                ret_str.Append(TESIX_ColorStringPair(GetNodeString(), GetNodeColor(cur)));
                Next();
                ret_str.Append(TESIX_ColorStringPair(GetNodeInbetween(), TESIX_COLORS_NONE));
            } else break;
        } else {

            break;
        }
    }

    ret_str.Append(TESIX_ColorStringPair("|", TESIX_COLORS_NONE));

    return ret_str;
}

void TESIX_SyntaxTree::TEST(){
    while(!is_last){
    // std::cout << "\033[1;34mnode:\033[0m " << GetNodeString(cur) << " \033[1;31mtype:\033[0m " << ts_node_type(*node) << "\n";
    std::cout << NextLine().GetStr();
    }
}

bool TESIX_SyntaxTree::IsMultiline(TSNode node){
    std::string node_string = GetNodeString(node);

    uint32_t index = node_string.find('\n', 0);
    std::cout << index << "\n";
    if(index == -1) return false;

    index = node_string.find('\n', index + 1);
    std::cout << index << "\n";
    if(index == -1) return false;

    return true;
}

bool TESIX_SyntaxTree::IsLineStart(TSNode node){
    std::string inbtw = GetNodeInbetween();
    if(TESIX_File::CharCount(inbtw, '\n') == 1) return true;
    else return false;
}

std::string TESIX_SyntaxTree::GetNodeString(){
    return file->GetInterval(ts_node_start_byte(cur), ts_node_end_byte(cur) - 1);
}

std::string TESIX_SyntaxTree::GetNodeString(TSNode node){
    return file->GetInterval(ts_node_start_byte(node), ts_node_end_byte(node) - 1);
}

std::string TESIX_SyntaxTree::GetNodeInbetween(){
    return file->GetInterval(ts_node_end_byte(last_token), ts_node_start_byte(cur) - 1);
}

std::optional<TSNode> TESIX_SyntaxTree::NextSibling(TSNode node){
    TSNode sibling = ts_node_next_sibling(node);

    while(ts_node_is_null(sibling)){
        node = ts_node_parent(node);
        if(ts_node_is_null(node)){
            return std::optional<TSNode>();
        }
        sibling = ts_node_next_sibling(node);
    }

    if(ts_node_is_missing(sibling)){
        errors.push_back(sibling);
        return NextSibling(ts_node_parent(node));
    }

    return std::optional<TSNode>(sibling);
}

bool TESIX_SyntaxTree::IsToken(TSNode node){
    std::string type(ts_node_type(node));

    bool is_token = false;
    
    for(std::string token_type : token_types){
        if(type == token_type) is_token = true;
    }
    
    return is_token;
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
