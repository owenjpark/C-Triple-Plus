#ifndef PARSE_H
#define PARSE_H

#include <vector>
#include "lex.h"

using namespace std;

class AST {

    public: 

    // AST nodes 
    struct node{

        string data;
        vector<node*> children;
    };

    node* root; 
    // Constructor and Destructor
    AST();
    ~AST();
    // AST functions
    string equation();
    float answer();

    //may have to make some helper functions in private
    private:
    // reader function 
    // insert function
        void insert(){}

};



#endif