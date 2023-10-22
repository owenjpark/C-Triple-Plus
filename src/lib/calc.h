#include "lex.h"
#include <iostream>
#include <string>
#include <vector>

class AST {

    public:

    struct Node {
        string data;
        
        Node * leftChild;
        Node * rightChild;
    };

    AST();
    ~AST();
    Node* root;

    private:

    void destructor(Node* node);
};

    AST::Node* build(AST::Node* root, vector<token> vec);
    string stringAST(AST::Node* root);
    float result(AST::Node* root);

    // helper functions 
    int precedence(vector<token> vec);

    