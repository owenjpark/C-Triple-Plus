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
        
};

struct definedVar {
    string ID;
    double value;
};

AST::node* createAST(vector<token> tokenVec, int index);

void printInfix(AST::node* someNode);

double evaluateAST(AST::node* someNode);

void expressionChecker(int i, vector<token> &tokenVec);

// AST parser(int i, vector<token> tokenVec, vector<string> definedVars);

#endif