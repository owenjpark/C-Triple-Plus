#ifndef CALC_H
#define CALC_H

#include "lex.h"
#include <memory>
#include <variant>

struct Value: public variant <double, bool, shared_ptr<vector<Value>>, string> { // value of element of array; NOTE: string if null
    using variant<double, bool, std::shared_ptr<std::vector<Value>>, string>::variant;
    bool operator == (const Value& other) const {
        // Check if the types are the same
        if (index() != other.index()) {
            return false;
        }

        if (index() == 0) {
            return (get<double>(*this) == get<double>(other));
        }
        else if (index() == 1) {
            return (get<bool>(*this) == get<bool>(other));
        }
        else if (index() == 3) {
            return (get<string>(*this) == get<string>(other));
        }
        else { // comparing arrays
            shared_ptr<vector<Value>> thisArray = get<shared_ptr<vector<Value>>>(*this);
            shared_ptr<vector<Value>> otherArray = get<shared_ptr<vector<Value>>>(other);
            if (thisArray->size() != otherArray->size()) {
                return false;
            }
            for (unsigned i = 0; i < thisArray->size(); i++) {
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
                this->data = data;
                this->array = array;
                this->type = type;
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
    shared_ptr<std::vector<Value>> arrayValue = make_shared<std::vector<Value>>();

    string type;
};

struct boolNum { // return type for evaluating AST2
    boolNum(double mNum = 0, bool mBool = false, string mType = "", string mNull = "") {
        this->mNum = mNum;
        this->mBool = mBool;
        this->mNull = mNull;
        this->mType = mType;
    }
    double mNum;
    bool mBool;
    string mNull;
    shared_ptr<std::vector<Value>> mArray = make_shared<std::vector<Value>>();

    string mType; // indicates which type it's returning
};

shared_ptr<AST2::Node> build(vector<token> vec, token parentToken);

void printInfix2(shared_ptr<AST2::Node> &someNode);

boolNum evaluate(shared_ptr<AST2::Node> &root, vector<variable> &variables);

// helper functions 

int parenthChecker(unsigned i, vector<token> tokenVec);

int bracChecker(unsigned i, vector<token> tokenVec);

void expressionChecker(unsigned startIndex, unsigned endIndex, vector<token> tokenVec);

double precedence(vector<token> vec);

bool stob(string data);

void arrayPrinter(shared_ptr<std::vector<Value>> array);

#endif