#ifndef CALC_H
#define CALC_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

 struct vecComponent {
        string data;
        int column;
        int row;

    };
class AST {

    public: 

    // AST nodes 
    struct token{

        string data;
        vector<token*> children;
    };

    token* root; 
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