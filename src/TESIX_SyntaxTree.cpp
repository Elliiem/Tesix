#include "TESIX_SyntaxTree.h"

TESIX_SyntaxTree::TESIX_SyntaxTree(TSNode root_node){
    cur = root_node;
}

TESIX_SyntaxTree::~TESIX_SyntaxTree(){

}

TSNode *TESIX_SyntaxTree::Next(){
    last = cur;

    if(ts_node_child_count(cur) != 0){
        cur = ts_node_child(cur, 0);
    } else {
        std::optional<TSNode> next = NextSibling(cur);
            
        if(!next.has_value() || ts_node_is_missing(next.value())){
            return nullptr;
        }
        cur = next.value();
    }

    return &cur;
}

TSNode *TESIX_SyntaxTree::Last(){
    TSNode tmp = cur;
    cur = last;
    last = tmp;
    return &cur;
}

TSNode *TESIX_SyntaxTree::NextToken(){
    if(Next() == nullptr) return nullptr;
    
    while(!IsToken(cur)){
        if(Next() == nullptr) return nullptr;
    }

    return &cur;
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
