#ifndef CALC_H
#define CALC_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

/*
struct AST {
    ASTnode *root;

    prints equation 
    solves equation \ prints answer 

    addnode
    AST()      // constructor
    ~AST()     // destructor 
};

< readme.txt

struct ASTnode {
    AST* leftchild;
    AST* rightchild;
    std::string text;
    
    virtual float operation(AST* left, AST* right);
};
*/

struct vecComponent {
    string data;
    int column;
    int row;

};



#endif