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

AST::node* parse(vector<vecComponent> lexVec, int index);

void printEquation(AST::node* nodeParam);

double evaluate(AST::node* nodeParam);



#endif