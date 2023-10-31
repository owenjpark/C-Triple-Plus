
#include "calc.h"
#include "lex.h"

#include <string.h>
#include <cmath>

AST2::AST2() {
    root = nullptr;
};

AST2::~AST2() {
}

int precedence(vector<token> vec) {
    // PRESCEDENCE AS FOLLOWS
    // "="                0
    // "|"                1 
    // "^"                2
    // "&"                3
    // "==" "!="          4
    // "<" "<=" ">" ">="  5
    // "+" "-"            6
    // "*" "/" "%"        7
    // "(" ")"            8

   int currLowestRating = 10; // initialize to any value above 4 (higest precedence)
   int leastPrecedenceIndex;
   int currPrecedence;

   int i = 0;
   int size = vec.size();
   
    while (i < size) {
        if (vec[i].data == "=") {
            currPrecedence = 0;
        }
        else if (vec[i].data == "|") {
            currPrecedence = 1;
        }
        else if (vec[i].data == "^") {
            currPrecedence = 2;
        }
        else if (vec[i].data == "&") {
            currPrecedence = 3;
        }
        else if (vec[i].data == "==" || vec[i].data == "!=") {
            currPrecedence = 4;
        }
        else if (vec[i].data == "<" || vec[i].data == "<=" || vec[i].data == ">" || vec[i].data == ">=") {
            currPrecedence = 5;
        }
        else if (vec[i].data == "+" || vec[i].data == "-") {
            currPrecedence = 6;
        }
        else if (vec[i].data == "*" || vec[i].data == "/" || vec[i].data == "%") {
            currPrecedence = 7;
        }
        else if (vec[i].data == "(" || vec[i].data == ")") {
            currPrecedence = 8;
            while (vec[i].data != ")" && i < int(vec.size())) { // going to the index )
                i++;
            }
        } 
        else { // else its a number, variable, or bool
            currPrecedence = 9;
        }
        if (currPrecedence <= currLowestRating) {
            if (currLowestRating == 0 && currPrecedence == 0) {
                // do nothing if seeing another "=" bc it's right associative
            }
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
    if (vec.at(leastPrecedenceIndex).type == "num" || vec.at(leastPrecedenceIndex).type == "var" || vec.at(leastPrecedenceIndex).type == "lParenth") {
        token errorToken = vec.at(1);
        error rParenthError(errorToken.data, errorToken.row, errorToken.column, 2);
        throw rParenthError;
    }
    return leastPrecedenceIndex;
}

unique_ptr<AST2::Node> build(vector<token> vec, token parentToken) {
    if (vec.size() == 1 || (vec.size() == 2 && vec.at(1).type == "end")) {
        if (vec.at(0).type == "num" || vec.at(0).type == "var" || vec.at(0).type == "bool") { // BASE CASE: vec has only num, variable, or bool
            unique_ptr<AST2::Node> node(new AST2::Node);
            node->data = vec.at(0).data;
            node->type = vec.at(0).type;
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
        }
        else if (vec.at(0).type == "end") { // vec empty
            error empty("END", vec.at(0).row, vec.at(0).column, 2);
            throw(empty);
        }
        else if (vec.at(0).type == "lParenth") { // SPECIAL CASE: "(" error has to be column 2
            token errorToken = vec.at(1);
            error noFirstOperand (errorToken.data, errorToken.row, errorToken.column, 2);
            throw noFirstOperand;
        }
        else { // else its not num, variable, or bool
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
        if ((vec.size() - 1) > i) { // more indexes past i, check if next index is end
            if (vec.at(i + 1).type == "end") {
                if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq" || vec.at(i - 1).type == "eqIneq" || vec.at(i - 1).type == "logicOp") {
                    token errorToken = vec.at(i);
                    error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                    throw parenthNumEnd;
                }
                vec.erase(vec.begin() + i); // NOTE: have to delete end
                vec.erase(vec.begin());
            }
        }
        else { // no indexes past i
            if (vec.at(i).type != "rParenth") {
                token errorToken = vec.at(i);
                error noClosingParenth(errorToken.data, errorToken.row, errorToken.column, 2);
                throw noClosingParenth;
            }
            if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq" || vec.at(i - 1).type == "eqIneq" || vec.at(i - 1).type == "logicOp") { // TODO: this is not used
                token errorToken = vec.at(i - 1);
                error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                throw parenthNumEnd;
            }
            vec.pop_back();
            vec.erase(vec.begin());
        }
    }

    // we have an expresion of at least 1 operation & stripped of ()
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
        cout << "here" << endl;
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
    if (someNode->type == "op" || someNode->type == "eq" || someNode->type == "eqIneq" || someNode->type == "logicOp") {
        cout << "(" ;
    }

    if (someNode->leftChild != nullptr && someNode->rightChild != nullptr) {
        printInfix2(someNode->leftChild);
        cout << " " << someNode->data << " ";
        printInfix2(someNode->rightChild);
    }

    if (someNode->type == "op" || someNode->type == "eq" || someNode->type == "eqIneq" || someNode->type == "logicOp") {
        cout << ")" ;
    }
    else if (someNode->type == "var" || someNode->type == "bool") {
        cout << someNode->data;
    }
    else { // else its a number
        double num = stod(someNode->data);
        cout << num;
    }
}

bool stob(string data) { // stob = "string to double"; helper function for evaluate
    if (data == "true") {
        return true;
    }
    else {
        return false;
    }
}

boolNum evaluate(unique_ptr<AST2::Node> &root, vector<variable> &variables){ 
    if (root->leftChild == nullptr && root->rightChild == nullptr) { // BASE CASE: when data is number, variable, or bool
        if (root->type == "var") { // if its a var
            bool assigned = false;
            if (variables.size() > 0) {
                for (int i = 0; i < int(variables.size()); i++){
                    if (variables[i].name == root->data) {
                        assigned = true;
                        if (variables[i].type == "bool") {
                            boolNum varValue(0, variables[i].boolValue, "bool");
                            return varValue;
                        }
                        else { // else its a num
                            boolNum varValue(variables[i].numValue, 0, "num");
                            return varValue;
                        }
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
        else if (root->type == "bool") { // if its a bool
            boolNum boolVal(0, stob(root->data), "bool");
            return boolVal;
        }
        else { // else its a num
            boolNum numVal(stod(root->data), 0, "num");
            return numVal;
        }
    }

    // must be an operator
    if (root->data == "=") {
        boolNum result;
        result = evaluate(root->rightChild, variables);
        if (result.mType == "bool") { // assignment to bool e.g. x = true
            variable var(root->leftChild->data, 0, result.mBool, "bool");
            if (variables.size() == 0) variables.push_back(var);
            else {
                bool update = false;
                for (int i = 0; i < int(variables.size()); i++) {
                    if (variables[i].name == var.name) {
                        variables[i].boolValue = result.mBool;
                        update = true;
                    }
                }
                if (!update) variables.push_back(var);
            }
        }
        else { // else assignment to num e.g. x = 12;
            variable var(root->leftChild->data, result.mNum, 0, "num");
            if (variables.size() == 0) variables.push_back(var);
            else {
                bool update = false;
                for (int i = 0; i < int(variables.size()); i++) {
                    if (variables[i].name == var.name) {
                        variables[i].numValue = result.mNum;
                        update = true;
                    }
                }
                if (!update) variables.push_back(var);
            }
        }
        return result;
    }
    else if (root->type == "op") {
        if (evaluate(root->leftChild, variables).mType != "num" || evaluate(root->rightChild, variables).mType != "num") {
            error invalidReturn;
            invalidReturn.code = 4;
            throw(invalidReturn);
        }

        if (root->data == "+") {
            boolNum result(0, false, "num");
            result.mNum = evaluate(root->leftChild, variables).mNum + evaluate(root->rightChild, variables).mNum;
            return result;
        }
        else if (root->data == "-") {
            boolNum result(0, false, "num");
            result.mNum = evaluate(root->leftChild, variables).mNum - evaluate(root->rightChild, variables).mNum;
            return result;
        }
        else if (root->data == "*") {
            boolNum result(0, false, "num");
            result.mNum = evaluate(root->leftChild, variables).mNum * evaluate(root->rightChild, variables).mNum;
            return result;
        }
        else if (root->data == "/") {
            double right = evaluate(root->rightChild, variables).mNum;
            if (right == 0) {
                error zero;
                zero.code = 0;
                throw(zero);
            }
            boolNum result(0, false, "num");
            result.mNum = evaluate(root->leftChild, variables).mNum / evaluate(root->rightChild, variables).mNum;
            return result;
        }
        else if (root->data == "%") { 
            double right = evaluate(root->rightChild, variables).mNum;
            if (right == 0) {
                error zero;
                zero.code = 0;
                throw(zero);
            }
            boolNum result(0, false, "num");
            result.mNum = fmod(evaluate(root->leftChild, variables).mNum, evaluate(root->rightChild, variables).mNum);
            return result;
        }
        else if (root->data == "<") {
            boolNum result(0, false, "bool");
            if (evaluate(root->leftChild, variables).mNum < evaluate(root->rightChild, variables).mNum) {
                result.mBool = true;
            }
            else {
                result.mBool = false;
            }
            return result;
        }
        else if (root->data == "<") {
            boolNum result(0, false, "bool");
            result.mBool = evaluate(root->leftChild, variables).mNum < evaluate(root->rightChild, variables).mNum;
            return result;
        }
        else if (root->data == ">") {
            boolNum result(0, false, "bool");
            result.mBool = evaluate(root->leftChild, variables).mNum > evaluate(root->rightChild, variables).mNum;
            return result;
        }
        else if (root->data == "<=") {
            boolNum result(0, false, "bool");
            result.mBool = evaluate(root->leftChild, variables).mNum <= evaluate(root->rightChild, variables).mNum;
            return result;
        }
        else if (root->data == ">="){
            boolNum result(0, false, "bool");
            result.mBool = evaluate(root->leftChild, variables).mNum >= evaluate(root->rightChild, variables).mNum;
            return result;
        }
    }
    else if (root->type == "logicOp") {
        if (evaluate(root->leftChild, variables).mType != "bool" || evaluate(root->rightChild, variables).mType != "bool") {
            error invalidReturn;
            invalidReturn.code = 4;
            throw(invalidReturn);
        }

        if (root->data == "&") {
            boolNum result(0, false, "bool");
            result.mBool = evaluate(root->leftChild, variables).mBool && evaluate(root->rightChild, variables).mBool;
            return result;
        }
        else if (root->data == "^") {
            boolNum result(0, false, "bool");
            result.mBool = evaluate(root->leftChild, variables).mBool ^ evaluate(root->rightChild, variables).mBool;
            return result;
        }
        else if (root->data == "|") {
            boolNum result(0, false, "bool");
            result.mBool = evaluate(root->leftChild, variables).mBool || evaluate(root->rightChild, variables).mBool;
            return result;
        }
    }
    else if (root->type == "eqIneq") { // type is "eqIneq"
        if (evaluate(root->leftChild, variables).mType == "bool") {
            if (evaluate(root->rightChild, variables).mType != "bool") {
                error invalidReturn;
                invalidReturn.code = 4;
                throw(invalidReturn);
            }

            if (root->data == "==") {
                boolNum result(0, false, "bool");
                result.mBool = evaluate(root->leftChild, variables).mBool == evaluate(root->rightChild, variables).mBool;
                return result;
            }
            else if (root->data == "!="){ // else its "!="
                boolNum result(0, false, "bool");
                result.mBool = evaluate(root->leftChild, variables).mBool != evaluate(root->rightChild, variables).mBool;
                return result;
            }
        }
        else { // else left child is a num
            if (evaluate(root->rightChild, variables).mType != "num") {
                error invalidReturn;
                invalidReturn.code = 4;
                throw(invalidReturn);
            }
            
            if (root->data == "==") {
                boolNum result(0, false, "bool");
                result.mBool = evaluate(root->leftChild, variables).mNum == evaluate(root->rightChild, variables).mNum;
                return result;
            }
            else if (root->data == "!="){
                boolNum result(0, false, "bool");
                result.mBool = evaluate(root->leftChild, variables).mNum != evaluate(root->rightChild, variables).mNum;
                return result;
            }
        }
    }
    boolNum someBoolNum; // to avoid reaching end of non-void function warning
    return someBoolNum;
}