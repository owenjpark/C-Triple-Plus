#ifndef AST_H
#define AST_H

#include <vector>
#include "lex.h"

using namespace std;

class AST {

    public: 
        struct node{
            string data;
            string parent;
            vector<node*> children;
        };

        node* root; 

        AST();
        ~AST();

        void printEquation(AST::node* nodeParam);
        float answer(AST);
};

AST::node* parse(vector<vecComponent> lexVec, int index, string parent);

double evaluate(AST::node* nodeParam);

void printEquation(AST::node* nodeParam);

void lexer (const string line, const int row, vector<vecComponent> &inputVec);

#endif