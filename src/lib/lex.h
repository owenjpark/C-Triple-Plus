#ifndef CALC_H
#define CALC_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct linkedList {
    Node* root;
    
    linkedList();
    ~linkedList();
};

struct Node {
    string data;
    int column;
    int row;

    Node* next;
};

linkedList lexer () { //todo
};

#endif