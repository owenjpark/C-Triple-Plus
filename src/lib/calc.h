#ifndef CALC_H
#define CALC_H

#include "lex.h"
#include <memory>
#include <variant>

class AST3 { // AST for statements and expressions
    public:
        struct Node {
            string type;

            string data;
            vector<shared_ptr<Node>> array;
            
            vector<shared_ptr<AST3::Node>> children;
        };

        shared_ptr<Node> root;
};

struct Value: public variant <double, bool, string, shared_ptr<vector<Value>>> { // value of element of array; NOTE: string "null" storing null
    using variant<double, bool, string, shared_ptr<vector<Value>>>::variant;

    bool operator == (const Value& other) const {
        if (index() != other.index()) { // check storing same type
            return false;
        }

        if (index() == 0) {
            return (get<double>(*this) == get<double>(other));
        }
        else if (index() == 1) {
            return (get<bool>(*this) == get<bool>(other));
        }
        else if (index() == 2) {
            return (get<string>(*this) == get<string>(other));
        }
        else { // comparing arrays
            shared_ptr<vector<Value>> thisArray = get<shared_ptr<vector<Value>>>(*this);
            shared_ptr<vector<Value>> otherArray = get<shared_ptr<vector<Value>>>(other);
            if (thisArray->size() != otherArray->size()) { // check if same size
                return false;
            }
            for (unsigned i = 0; i < thisArray->size(); i++) { // compare each element of array
                if (thisArray->at(i) != otherArray->at(i)) {
                    return false;
                }
            }
            return true;
        }
    }

    bool operator != (const Value& other) const {
        return !(*this == other);
    }
}; 

class AST2 { // AST for expressions
    public:
        struct Node {
            Node (string data = "", vector<shared_ptr<Node>> array = {}, string type = "", shared_ptr<Node> leftChild = nullptr, shared_ptr<Node> rightChild = nullptr) {
                this->type = type;
                this->data = data;
                this->array = array;
                this->leftChild = leftChild;
                this->rightChild = rightChild;
            }
            string type;

            string data; // stores string of data; for all types except for "array"
            vector<shared_ptr<Node>> array; // if type is array store each element of array in vector of nodes

            shared_ptr<Node> leftChild;
            shared_ptr<Node> rightChild;
        };

        shared_ptr<Node> root;
};

struct variable; // forward declare

struct functionVal {
    shared_ptr<AST3::Node> statements;
    vector<variable> localScope;
};

struct variable {
    variable(string type = "", string name = "", double numValue = 0, bool boolValue = false) {
        this->type = type;
        this->name = name;
        this->numValue = numValue;
        this->boolValue = boolValue;
    }
    string type; // NOTE: for null, doesn't store anything, variable will just have type "null"
    string name; // stores name of name of variable

    double numValue;
    bool boolValue;
    shared_ptr<vector<Value>> arrayValue = make_shared<vector<Value>>(); // only for arrays
    functionVal funcVal; // only for functions
};

struct boolNum { // return type for evaluating AST2
    boolNum(string mType = "", double mNum = 0, bool mBool = false) {
        this->mNum = mNum;
        this->mBool = mBool;
        this->mType = mType;
    }
    string mType; // indicates which type it's returning; NOTE: for null, doesn't store anything, boolNum will just have type "null"

    double mNum;
    bool mBool;
    shared_ptr<vector<Value>> mArray = make_shared<vector<Value>>();
};


// functions
shared_ptr<AST2::Node> build(vector<token> vec);

void printInfix2(shared_ptr<AST2::Node> &someNode);

boolNum evaluate(shared_ptr<AST2::Node> &root, vector<variable> &variables);


// helper functions 
int parenthChecker(unsigned i, vector<token> tokenVec);

int brackChecker(unsigned i, vector<token> tokenVec);

int paramChecker(unsigned i, vector<token> tokenVec);

void expressionChecker(unsigned startIndex, unsigned endIndex, vector<token> tokenVec);

double precedence(vector<token> vec);

bool stob(string data);

void arrayPrinter(shared_ptr<vector<Value>> array);

#endif