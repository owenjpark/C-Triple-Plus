#ifndef CALC_H
#define CALC_H

#include "lex.h"
#include <memory>

class AST2 {
    public:
        struct Node {
            string data;
            string type;
            
            unique_ptr<Node> leftChild;
            unique_ptr<Node> rightChild;
        };

        AST2();
        ~AST2();

        unique_ptr<Node> root;
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

struct boolNum {
    boolNum(double mNum = 0, bool mBool = false, string mType = "") {
        this->mNum = mNum;
        this->mBool = mBool;
        this->mType = mType;
    }
    double mNum;
    bool mBool;

    string mType;
};

unique_ptr<AST2::Node> build(vector<token> vec, token parentToken);
void printInfix2(unique_ptr<AST2::Node> &someNode);
boolNum evaluate(unique_ptr<AST2::Node> &root, vector<variable> & variables);

// helper functions 

bool stob(string data);

int precedence(vector<token> vec);

#endif