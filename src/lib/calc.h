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
    float value;
};

unique_ptr<AST2::Node> build(vector<token> vec, token parentToken);
string infixString(unique_ptr<AST2::Node> &root, string equation = "");
float evaluate(unique_ptr<AST2::Node> &root, vector<variable> & variables, float result=0);

// helper functions 

int precedence(vector<token> vec);

int findMatchingParenth(int i, vector<token> tokenVec);

void expressionChecker2(unsigned startIndex, unsigned endIndex, bool inNested, vector<token> tokenVec);

#endif