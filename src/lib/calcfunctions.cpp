#include "calc.h"
#include "lex.h"

AST2::AST2() {
    root = nullptr;
};

AST2::~AST2() {
}

int findMatchingParenth(int i, vector<token> tokenVec) { // (12 + 7) should start at 12, returns index of )
    int parenthDiff = 1;
    while (parenthDiff != 0 && !(tokenVec.at(i).type == "end")) {
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
        if (parenthDiff == 0) { // so it doesn't over iterate
            break;
        }
        i++;
    }
    if (tokenVec.at(i).type == "end") {
        error someError(tokenVec.at(i).data, 1, tokenVec.at(i).column, 2);
        throw someError;
    }

    return i;
}

void expressionChecker2(unsigned startIndex, unsigned endIndex, bool inNested, vector<token> tokenVec) {
    if (tokenVec.size() == 1) { // only end token
        error someError(tokenVec.at(0).data, 1, tokenVec.at(0).column, 2);
        throw someError;
    }
    // has to have at least 1 real token in it
    if (tokenVec.at(startIndex).type != "num" && tokenVec.at(startIndex).type != "var" && tokenVec.at(startIndex).type != "lParenth") { // doesn't start with big 3
        error someError(tokenVec.at(startIndex).data, 1, tokenVec.at(startIndex).column, 2);
        throw someError;
    }
    // at least 1 element
    for (unsigned i = startIndex; i < endIndex; i++) { // doesn't hit end token
        if (tokenVec.at(i).type == "lParenth") {
            i++; // skip "("
            int endParenthIndex = findMatchingParenth(i, tokenVec);
            expressionChecker2(i, endParenthIndex, true, tokenVec);
            i = endParenthIndex;
            
            if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "end" && tokenVec.at(i + 1).type != "rParenth") { // after lParenth
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "num" || tokenVec.at(i).type == "var") {
            if (!inNested) { // edge case: only allow ")" in nested expression
                if (tokenVec.at(i + 1).type == "rParenth") {
                    error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                    throw someError;
                }
            }
            if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "end" && tokenVec.at(i + 1).type != "rParenth") {
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "op") {
            if (tokenVec.at(i + 1).type != "num" && tokenVec.at(i + 1).type != "var" && tokenVec.at(i + 1).type != "lParenth") {
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "eq") {
            if (tokenVec.at(i - 1).type != "var") {
                error someError(tokenVec.at(i).data, 1, tokenVec.at(i).column, 2);
                throw someError;
            }
            if (tokenVec.at(i + 1).type != "num" && tokenVec.at(i + 1).type != "var" && tokenVec.at(i + 1).type != "lParenth") {
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
    }
}

int precedence(vector<token> vec) {
    // PRECEDENCE RULES AS FOLLOWS:
    // "="      0
    // "+" "-"  1 
    // "*" "/"  2
    // "("      3
    // else     4

   int currLowestRating = 10; // initialize to any value above 4 (higest precedence)
   int leastPrecedenceIndex;
   int currPrecedence;

   int i = 0;
   int size = vec.size();
   
    while (i < size) {
        if (vec[i].data == "=") {
            currPrecedence = 0;
        }
        else if (vec[i].data == "+" || vec[i].data == "-") {
            currPrecedence = 1;
        }
        else if( vec[i].data == "*" || vec[i].data == "/") {
            currPrecedence = 2;
        }
        else if(vec[i].data == "(") {
            currPrecedence = 3;
            while (vec[i].data != ")" && i < int(vec.size())) { // going to the index of ")"
                i++;
            }
        }
        else { // if its a number or variable
            currPrecedence = 4;
        }

        if (currPrecedence <= currLowestRating) {
            if (currLowestRating == 0 && currPrecedence == 0) {
                // for assignment operator (right associativity); do nothing
            } 
            else {
                currLowestRating = currPrecedence;
                leastPrecedenceIndex = i;
            }
        }

        i++;
    }
    return leastPrecedenceIndex;
}

unique_ptr<AST2::Node> build(vector<token> vec) {
    if (vec.size() == 1 || (vec.size() == 2 && vec.at(1).type == "end")) {
        if (vec.at(0).type == "num" || vec.at(0).type == "var") { // BASE CASE: vec has only num or variable (even if it includes END   )
            unique_ptr<AST2::Node> node(new AST2::Node);
            node->data = vec.at(0).data;
            node->type = vec.at(0).type;
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
        }
    }

    // case if argument is inside ()
    if (vec.at(0).data == "(") { // vec starts with "("
        unsigned i = 1; // go past parenthesis
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
            i++;
        }
        if ((vec.size() - 1) > i) { // more indexes past i
            if (vec.at(i + 1).type == "end") {
                vec.erase(vec.begin() + i); // NOTE: deleting end first
                vec.erase(vec.begin());
            }
        }
        else {
            // TODO what if parenthesis never closes?
            vec.pop_back();
            vec.erase(vec.begin());
        }
    }

    int lowestPrecedenceIndex = precedence(vec);
    unique_ptr<AST2::Node> oper(new AST2::Node);
    oper->data = vec.at(lowestPrecedenceIndex).data;
    oper->type = vec.at(lowestPrecedenceIndex).type;

    vector<token> leftVec; // adding left children
    for (int j = 0; j < lowestPrecedenceIndex; j++) {
        leftVec.push_back(vec[j]);
    }
    oper->leftChild = (build(leftVec));
    
    vector<token> rightVec; // adding right children
    int end = vec.size(); 
    for (int i = lowestPrecedenceIndex + 1; i < end; i++) {
        rightVec.push_back(vec[i]);
    }
    oper->rightChild = (build(rightVec));
    
    return oper;
}

string infixString(unique_ptr<AST2::Node> &root, string equation) {
    if (root->leftChild == nullptr && root->rightChild == nullptr) { //base case num or variable
        equation += root->data;
    }
    if (root->type == "op" || root->type == "eq") {
        return "(" + infixString(root->leftChild) + " " + root->data + " " + infixString(root->rightChild) + ")";
    }

    return equation;
}

float evaluate(unique_ptr<AST2::Node> &root, vector<variable> &variables, float result){ 
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
            float right = evaluate(root->rightChild, variables);
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