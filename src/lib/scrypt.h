#ifndef SCRYPT_H
#define SCRYPT_H

#include "lex.h"
#include "calc.h"


class AST3 { 
    public:
        struct Node {
            string data;
            string type;
            
            vector<unique_ptr<AST3::Node>> children;
        };

        AST3();
        ~AST3();

        unique_ptr<Node> root = std::make_unique<AST3::Node>();
};

struct ifTrue {
    int index;
    bool ft;
};

unique_ptr<AST3::Node> buildProgram(vector<token> &vec);
void runProgram(unique_ptr<AST3::Node> &node, vector<variable> &variables);

// helper functions
unique_ptr<AST3::Node> ConvertAST2ToAST3(unique_ptr<AST2::Node> &node2);
unique_ptr<AST2::Node> ConvertAST3ToAST2(unique_ptr<AST3::Node> &node3);
bool enterStatement (unique_ptr<AST3::Node> &root, vector<variable> &variables);

#endif