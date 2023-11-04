#include "lex.h"
#include "scrypt.h"
#include "calc.h"



// converting AST2 to AST3 to store in the same tree
std::unique_ptr<AST3::Node> ConvertAST2ToAST3(std::unique_ptr<AST2::Node> node2) {
    unique_ptr<AST3::Node> node3 = std::make_unique<AST3::Node>();
    node3->data = node2->data;
    node3->type = node2->type;

    if (node2->leftChild) {
        node3->children.push_back(ConvertAST2ToAST3(std::move(node2->leftChild)));
    }

    if (node2->rightChild) {
        node3->children.push_back(ConvertAST2ToAST3(std::move(node2->rightChild)));
    }

    return node3;
}

// also has to take in a vector of variables to be able to use if needed
unique_ptr<AST3::Node> buildProgram(vector<token> vec){
    // takes in vector and recursively creates an AST
    std::unique_ptr<AST3::Node> node = std::make_unique<AST3::Node>(); 
    return node;
    for (int i = 0; i < int(vec.size()); i++) {
        /*
        if (vec[i].type == "condition") {
            node->type = "condition";
            if (vec[i].data == "if" || vec[i].data == "while" || vec[i].data == "else if") {
                node->data = vec[i].data; 
                token parent = vec[i];


                // getting conditions 
                vector<token> expression;
                while(vec[i].data != "{") {
                    expression.push_back(vec[i]);
                    i++;
                }
    
                std::unique_ptr<AST2::Node> expressTree = build(expression, parent);
                node->children.push_back(ConvertAST2ToAST3(std::move(expressTree))); //converting AST2 to AST3
               
                // call build on vec i+1 until {
                // save vec into children 
                // call buildprogram on { to }
            }
            
            else if (vec[i].data == "else") {
                while(vec[i].data != "{") {
                    i++;
                }
            }
            // getting what is inside of {}
            vector<token> actions;
            while(vec[i].data != "}") {
                actions.push_back(vec[i]);
                i++;
            }
            node->children.push_back(buildProgram(actions));
        }

        // expressions 
        else if (vec[i].type == "var") {
            // grab the entire expression
            int row = vec[i].row;
            vector<token> express;
            while (vec[i].row == row) {
                express.push_back(vec[i]);
                i++;
            }
            token parent = vec[i];
            std::unique_ptr<AST2::Node> treeExpress = build(express,parent);
            node->children.push_back(ConvertAST2ToAST3(std::move(treeExpress))); //converting AST2 to AST3
        }

        else if (vec[i].type == "print") {
            node->data = "print";
            node->type = "print";
            i++;
            std::unique_ptr<AST3::Node> printNode = std::make_unique<AST3::Node>();
            printNode->data = vec[i].data;
            printNode->type = vec[i].type;
            node->children.push_back(std::move(printNode)); 
        }
    return node; */
    } 
    
    //return node;
}; 
/* boolNum printFunc(unique_ptr<AST3::Node> &root){

}; */
