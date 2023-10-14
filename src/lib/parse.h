#ifndef PARSE_H
#define PARSE_H

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



#endif