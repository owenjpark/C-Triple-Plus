#ifndef CALC_H
#define CALC_H

#include "lex.h"
#include <memory>
#include <variant>

struct Value: public variant <double, bool, shared_ptr<vector<Value>>> { // value of element of array
    using variant<double, bool, std::shared_ptr<std::vector<Value>>>::variant;
}; 

class AST2 { // AST for expressions
    public:
        struct Node {
            Node (string data = "", vector<shared_ptr<Node>> array = {}, string type = "", shared_ptr<Node> leftChild = nullptr, shared_ptr<Node> rightChild = nullptr) {
                this->data = data;
                this->array = array;
                this->type = type;
                this->leftChild = leftChild;
                this->rightChild = rightChild;
            }
            string data;
            vector<shared_ptr<Node>> array;

            string type;
            
            shared_ptr<Node> leftChild;
            shared_ptr<Node> rightChild;
        };

        AST2();
        ~AST2();

        shared_ptr<Node> root;
};

struct variable {
    variable(string name = "", double numValue = 0, bool boolValue = false, string type = "") {
        this->name = name;
        this->numValue = numValue;
        this->boolValue = boolValue;
        this->type = type;
    }
    string name;

    double numValue;
    bool boolValue;

    string type;
};

struct boolNum { // return type for evaluating AST2
    boolNum(double mNum = 0, bool mBool = false, string mType = "") {
        this->mNum = mNum;
        this->mBool = mBool;
        this->mType = mType;
    }
    double mNum;
    bool mBool;
    shared_ptr<std::vector<Value>> mArray = make_shared<std::vector<Value>>();

    string mType; // indicates which type it's returning
};

shared_ptr<AST2::Node> build(vector<token> vec, token parentToken, vector<variable> &variables);

void printInfix2(shared_ptr<AST2::Node> &someNode);

boolNum evaluate(shared_ptr<AST2::Node> &root, vector<variable> &variables);

// helper functions 

bool stob(string data);

int precedence(vector<token> vec);

void arrayPrinter(shared_ptr<std::vector<Value>> array);

#endif