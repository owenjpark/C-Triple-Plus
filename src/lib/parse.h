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

void printInfix(AST::node* nodeParam);

double evaluateAST(AST::node* nodeParam);

void expressionChecker(vector<token> tokenVec);

AST parser(vector<token> tokenVec);

#endif