#include "calc.h"
#include "lex.h"

#include <string.h> 

AST2::AST2() {
    root = nullptr;
};

AST2::~AST2() {
}

int precedence(vector<token> vec) {
    // PRESCEDENCE AS FOLLOWS
    // "="      0
    // "+" "-"  1 
    // "*" "/"  2
    // "(" ")"  3

   int currLowestRating = 10; // initialize to any value above 4 (higest precedence)
   int leastPrecedenceIndex;
   int currPrecedence;

   int i = 0;
   int size = vec.size();
   
    while (i < size) {
        if (vec[i].data == "=") currPrecedence = 0;
        else if (vec[i].data == "+" || vec[i].data == "-") {
            currPrecedence = 1;
        }
        else if( vec[i].data == "*" || vec[i].data == "/") {
            currPrecedence = 2;
        }
        else if(vec[i].data == "(" || vec[i].data == ")") {
            currPrecedence = 3;
            // going to the index )
            while (vec[i].data != ")" && i < int(vec.size())) {
                i++;
            }
        }
        // if its a number or variable
        else {
            currPrecedence = 4;
        }
        if (currPrecedence <= currLowestRating) {
            // for assignment 
            if (currLowestRating == 0 && currPrecedence == 0);
            else {
            currLowestRating = currPrecedence;
            leastPrecedenceIndex = i;
            }
        }

        i++;
    }
    if (vec.at(leastPrecedenceIndex).data == ")") {
        token errorToken = vec.at(leastPrecedenceIndex);
        error rParenthError(errorToken.data, errorToken.row, errorToken.column, 2);
        throw rParenthError;
    }
    return leastPrecedenceIndex;
}

unique_ptr<AST2::Node> build(vector<token> vec, token parentToken) {
    if (vec.size() == 1 || (vec.size() == 2 && vec.at(1).type == "end")) {
        if (vec.at(0).type == "num" || vec.at(0).type == "var") { // BASE CASE: vec has only num or variable (even if it includes END)
            unique_ptr<AST2::Node> node(new AST2::Node);
            node->data = vec.at(0).data;
            node->type = vec.at(0).type;
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
        }
        else if (vec.at(0).type == "end") { // vec empty
            error empty("END", 1, vec.at(0).column, 2);
            throw(empty);
        }
        else if (vec.at(0).type == "lParenth") { // we don't know at this point size is 1, so error at index 1
            token errorToken = vec.at(1);
            error noFirstOperand (errorToken.data, errorToken.row, errorToken.column, 2);
            throw noFirstOperand;
        }
        else {
            token errorToken = vec.at(0);
            error noFirstOperand(errorToken.data, errorToken.row, errorToken.column, 2);
            throw noFirstOperand;
        }
    }

    // case if argument is inside ()
    if (vec.at(0).data == "(") { // vec starts with "("
        unsigned i = 1; // go past parenthesis
        int paramCounter = 0;
        int parenthDiff = 1;

        while (parenthDiff != 0) {
            if (vec.at(i).type == "lParenth") {
                parenthDiff++;
            }
            else if (vec.at(i).type == "rParenth") {
                parenthDiff--;
            }
            if (i == vec.size() - 1 || parenthDiff == 0) { // break if i on last index
                break;
            }
            paramCounter++;
            i++;
        }
        // i should be at closing parenth or end of vector
        if (paramCounter < 1) {
            error emptyParenth(vec.at(i).data, vec.at(i).row, vec.at(i).column, 2);
            throw emptyParenth;
        }
        if ((vec.size() - 1) > i) { // more indexes past i
            if (vec.at(i + 1).type == "end") {
                if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq") {
                    token errorToken = vec.at(i);
                    error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                    throw parenthNumEnd;
                }
                vec.erase(vec.begin() + i); // NOTE: have to delete end first
                vec.erase(vec.begin());
            }
        }
        else {
            if (vec.at(i).type != "rParenth") {
                token errorToken = vec.at(i);
                error noClosingParenth(errorToken.data, errorToken.row, errorToken.column, 2);
                throw noClosingParenth;
            }
            if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq") { // TODO: this is not used
                token errorToken = vec.at(i - 1);
                error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                throw parenthNumEnd;
            }
            vec.pop_back();
            vec.erase(vec.begin());
        }
    }

    int lowestPrecedenceI = precedence(vec);
    
    unique_ptr<AST2::Node> oper(new AST2::Node);
    oper->data = vec.at(lowestPrecedenceI).data;
    oper->type = vec.at(lowestPrecedenceI).type;

    vector<token> leftVec;
    for (int j = 0; j < lowestPrecedenceI; j++) {
        leftVec.push_back(vec[j]);
    }
    if (leftVec.size() == 0) {
        token errorToken = vec.at(lowestPrecedenceI);
        error invalidOp(errorToken.data, errorToken.row, errorToken.column, 2);
        throw invalidOp;
    }
    oper->leftChild = build(leftVec, vec.at(lowestPrecedenceI));
    
    vector<token> rightVec;
    for (unsigned i = lowestPrecedenceI + 1; i < vec.size(); i++) {
        rightVec.push_back(vec[i]);
    }
    if (rightVec.size() == 0) {
        token errorToken = parentToken;
        error invalidOp(errorToken.data, errorToken.row, errorToken.column, 2);
        throw invalidOp;
    }

    if(vec.at(lowestPrecedenceI).type == "eq") {
        if (leftVec.size() != 1) {
            error invalidEQ(vec.at(lowestPrecedenceI).data, vec.at(lowestPrecedenceI).row, vec.at(lowestPrecedenceI).column, 2);
            throw invalidEQ;
        }
        if (leftVec.at(0).type != "var") {
            token errorToken = vec.at(lowestPrecedenceI);
            error invalidEq(errorToken.data, errorToken.row, errorToken.column, 2);
            throw invalidEq;
        }
    }

    oper->rightChild = build(rightVec, vec.at(lowestPrecedenceI));
    
    return oper;
}

void printInfix2(unique_ptr<AST2::Node> &someNode) {
    if (someNode->type == "op" || someNode->type == "eq") {
        cout << "(" ;
    }

    if (someNode->leftChild != nullptr && someNode->rightChild != nullptr) {
        printInfix2(someNode->leftChild);
        cout << " " << someNode->data << " ";
        printInfix2(someNode->rightChild);
    }

    if (someNode->type == "op" || someNode->type == "eq") {
        cout << ")" ;
    }
    else if (someNode->type == "var") {
        cout << someNode->data;
    }
    else { // else its a number
        double num = stod(someNode->data);
        cout << num;
    }
}

string infixString(unique_ptr<AST2::Node> &root, string equation) {
    if (root->leftChild == nullptr && root->rightChild == nullptr) { // base case num or variable
        equation += root->data;
    }
    if (root->type == "op" || root->type == "eq") {
        return "(" + infixString(root->leftChild) + " " + root->data + " " + infixString(root->rightChild) + ")";
    }

    return equation;
}

double evaluate(unique_ptr<AST2::Node> &root, vector<variable> &variables, double result){ 
    if (root->leftChild == nullptr && root->rightChild == nullptr) { // base case when data = number or variable
        if (root->type == "var") {
            bool assigned = false;
            if (variables.size() > 0) {
                for (int i = 0; i < int(variables.size()); i++){
                    if (variables[i].name == root->data) {
                        assigned = true;
                        return variables[i].value;
                    } 
                } 
            }
            if (!assigned) {
                error unassigned;
                unassigned.code = 3;
                unassigned.data = root->data;
                throw(unassigned); 
            }
        }
        else {
            result = stof(root->data);
            return result;
        }
    }

    if (root->data == "=") {
        result =  evaluate(root->rightChild, variables);
        variable var;
        var.name = root->leftChild->data;
        var.value = result; 
        if (variables.size() == 0 ) variables.push_back(var);
        else {
            bool update = false;
            for (int i = 0; i < int(variables.size()); i++) {
                if (variables[i].name == var.name) {
                    variables[i].value = result;
                    update = true;
                }
                
            }
            if (!update) variables.push_back(var);
            return result;
        }
    }
    else if (root->type == "op") {
        if (root->data == "+") {
            result = evaluate(root->leftChild, variables) + evaluate(root->rightChild, variables);
        }
        if (root->data == "-") {
            result = evaluate(root->leftChild, variables) - evaluate(root->rightChild, variables);
        }
        if (root->data == "*") {
            result = evaluate(root->leftChild, variables) * evaluate(root->rightChild, variables);
        }
        if (root->data == "/") {
            double right = evaluate(root->rightChild, variables);
            if (right == 0) {
                error zero;
                zero.code = 0;
                throw(zero);
            }
            result =  evaluate(root->leftChild, variables) / right;
        }
    }
    return result;
}