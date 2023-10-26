#ifndef PARSE_H
#define PARSE_H

#include <vector>
#include "lex.h"

using namespace std;

class AST {
    public: 
        struct node{
            string data;
            string type;
            vector<node*> children;
        };

        node* root; 

        AST();
        ~AST();
        void destructorHelper(AST::node* someNodeeter);      
};

struct definedVar {
    definedVar(string ID = "", double value = 0) {
        this->ID = ID;
        this->value = value;
    }

    string ID;
    double value;
};

AST::node* createAST(vector<token> tokenVec, int index);

void printInfix(AST::node* someNode);

double evaluateAST(AST::node* someNode, vector<definedVar> &definedVars);

void expressionChecker(int i, vector<token> &tokenVec);

void parser(vector<token> tokenVec);

#endif