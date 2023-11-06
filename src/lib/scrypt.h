
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


    int index;
    bool ft;
};
// functions 
unique_ptr<AST3::Node> buildProgram(vector<token> vec); //builder 
void runProgram(unique_ptr<AST3::Node> &node, vector<variable> &variables);
#endif