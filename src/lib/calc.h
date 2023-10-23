#include "lex.h"
#include <iostream>
#include <string>
#include <vector>

class AST2 {

    public:

    struct Node {
        string data;
        string type;
        
        Node* leftChild;
        Node* rightChild;
    };

    AST2();
    ~AST2();
    Node* root;

    void clear(Node* n);
};

struct error {
    string data;
    int column;
    int code;
}; 

    AST2::Node* build(AST2::Node* root, vector<token> vec);
    string stringAST2(AST2::Node* root, string equation = "");
    float evaluate(AST2::Node* root, float result=0);

    //void check(vector<token> vec);

    // helper functions 
    int precedence(vector<token> vec);

    