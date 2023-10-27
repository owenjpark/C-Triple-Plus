
#ifndef CALC_H
#define CALC_H

#include "lex.h"
#include <memory>

class AST2 { // named AST2 because conflicts w/AST from parse.cpp in gradescope
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

int findMatchingParenth(int i, vector<token> tokenVec); // helper function for expressionChecker2
void expressionChecker2(unsigned startIndex, unsigned endIndex, bool inNested, vector<token> tokenVec);

int precedence(vector<token> vec); // helper function for build
unique_ptr<AST2::Node> build(vector<token> vec);

string infixString(unique_ptr<AST2::Node> &root, string equation = ""); // creates a string in infix notation from AST

float evaluate(unique_ptr<AST2::Node> &root, vector<variable> & variables, float result = 0);

#endif