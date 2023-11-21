#ifndef CALC_H
#define CALC_H

#include "lex.h"
#include <memory>

class AST2 { // AST for expressions
    public:
        struct Node {
            string data;
            string type;
            
            shared_ptr<Node> leftChild;
            shared_ptr<Node> rightChild;
        };

        AST2();
        ~AST2();

        shared_ptr<Node> root;
};

struct variable {
    variable(string name = "", double numValue = 0, bool boolValue = false, string type = "") {
        this->name = name;
        this->numValue = numValue;
        this->boolValue = boolValue;
        this->type = type;
    }
    string name;
    double numValue;
    bool boolValue;
    string type;
};

struct boolNum { // return type for evaluating AST2
    boolNum(double mNum = 0, bool mBool = false, string mType = "") {
        this->mNum = mNum;
        this->mBool = mBool;
        this->mType = mType;
    }
    double mNum;
    bool mBool;

    string mType; // indicates which type it's returning
};

shared_ptr<AST2::Node> build(vector<token> vec);

void printInfix2(shared_ptr<AST2::Node> &someNode);

boolNum evaluate(shared_ptr<AST2::Node> &root, vector<variable> & variables);

// helper functions 

bool stob(string data);

int precedence(vector<token> vec);

#endif