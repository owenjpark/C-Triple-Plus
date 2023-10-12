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

struct Node {
    string data;
    int column;
    int row;

    Node* next = nullptr;
};

struct linkedList {
    Node* root;
    Node* latest;
    
    linkedList();
    ~linkedList();
    void lexer(const string line, const int row);
    void push(Node* node);
    void printer();
    Node* createNode(string data, int column, int row);
};



#endif