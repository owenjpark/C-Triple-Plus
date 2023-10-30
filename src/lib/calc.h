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
    string name;
    double value;
};

struct boolNum {
    boolNum(int mNum, bool mBool, string mType) {
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
boolNum evaluate(unique_ptr<AST2::Node> &root, vector<variable> & variables, boolNum result);

// helper functions 

int precedence(vector<token> vec);

int findMatchingParenth(int i, vector<token> tokenVec);

void expressionChecker2(unsigned startIndex, unsigned endIndex, bool inNested, vector<token> tokenVec);

#endif