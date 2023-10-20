#ifndef PARSE_H
#define PARSE_H

#include <vector>
#include "lex.h"

using namespace std;

class AST {
    public: 
        struct node{
            string data;
            vector<node*> children;
        };

        node* root; 

        AST();
        ~AST();
        
};

AST::node* createAST(vector<token> tokenVec, int index);

void printInfix(AST::node* someNode);

double evaluateAST(AST::node* someNode);

void expressionChecker(int i, vector<token> &tokenVec, vector<string> &definedVars);

AST parser(int i, vector<token> tokenVec, vector<string> definedVars);

#endif