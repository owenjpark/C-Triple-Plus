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

   int currLowestRating = 10; // initialize to any value above 9 (higest precedence)
   int leastPrecedenceIndex;
   int currPrecedence;

   unsigned i = 0;
   unsigned size = vec.size();
   
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
            if (i < vec.size() - 1) {
                i++;
                int parenthDiff = 1;
                while (parenthDiff != 0 && i != vec.size() - 1) {
                    if (vec[i].data == "(") {
                        parenthDiff++;
                    }
                    else if (vec[i].data == ")") {
                        parenthDiff--;
                    }
                    i++;
                }
                i--;
            }
            currPrecedence = 8;
        }
        else if (vec[i].data == "[" || vec[i].data == "]") {
            if (i < vec.size() - 1) {
                i++;
                int brackDiff = 1;
                while (brackDiff != 0 && i != vec.size() - 1) {
                    if (vec[i].data == "[") {
                        brackDiff++;
                    }
                    else if (vec[i].data == "]") {
                        brackDiff--;
                    }
                    i++;
                }
                i--;
            }
            currPrecedence = 8;
        }
        else { // else its a number, variable, bool, or null
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
    if (vec.at(0).type == "condition" || vec.at(0).type == "print") { // is this necessary? build function never sees these
        token errorToken = vec.at(0);
        error noStatement (errorToken.data, errorToken.row, errorToken.column, 2);
        // cout << "test1" << endl;
        throw noStatement;
    }
    if (currLowestRating > 7) { // no operators
        if (vec.at(0).data == "(") {
            int parenthDiff = 1;
            unsigned j = 1;
            while (parenthDiff != 0 && j != vec.size() - 1) {
                if (vec[j].data == "(") {
                    parenthDiff++;
                }
                else if (vec[j].data == ")") {
                    parenthDiff--;
                }
                j++;
            }
            token errorToken = vec.at(j);
            error noOperator(errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test2.5" << endl;
            throw noOperator;
        }
        else if (vec.at(0).data == "[") {
            int brackDiff = 1;
            unsigned j = 1;
            while (brackDiff != 0 && j != vec.size() - 1) {
                if (vec[j].data == "[") {
                    brackDiff++;
                }
                else if (vec[j].data == "]") {
                    brackDiff--;
                }
                j++;
            }
            token errorToken = vec.at(j);
            error noOperator(errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test2.5" << endl;
            throw noOperator;
        }

        // neither of two above cases, error on 2nd index
        token errorToken = vec.at(1);
        error noOperator(errorToken.data, errorToken.row, errorToken.column, 2);
        // cout << "test2" << endl;
        throw noOperator;
    }
    if (vec.at(leastPrecedenceIndex).data == ")") {
        token errorToken = vec.at(leastPrecedenceIndex);
        error rParenthError(errorToken.data, errorToken.row, errorToken.column, 2);
        // cout << "test3" << endl;
        throw rParenthError;
    }
    
    return leastPrecedenceIndex;
}

shared_ptr<AST2::Node> build(vector<token> vec, token parentToken) {
    if (vec.size() == 1 || (vec.size() == 2 && vec.at(1).type == "end")) {
        if (vec.at(0).type == "num" || vec.at(0).type == "var" || vec.at(0).type == "bool" || vec.at(0).type == "null") { // BASE CASE: vec has only num, variable, or bool
            shared_ptr<AST2::Node> node(new AST2::Node);
            node->data = vec.at(0).data;
            node->type = vec.at(0).type;
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
        }
        else if (vec.at(0).type == "end") { // vec empty
            error empty("END", 1, vec.at(0).column, 2);
            // cout << "test4" << endl;
            throw(empty);
        }
        else if (vec.at(0).type == "lParenth") { // SPECIAL CASE: "(" error has to be column 2
            token errorToken = vec.at(1);
            error noFirstOperand (errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test5" << endl;
            throw noFirstOperand;
        }
        else if (vec.at(0).type == "condition" || vec.at(0).type == "print") { 
            token errorToken = vec.at(0);
            error noStatement (errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test6" << endl;
            throw noStatement;
        }
        else { // else its not num, variable, bool, or null
            token errorToken = vec.at(0);
            error noFirstOperand(errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test7" << endl;
            throw noFirstOperand;
        }
    }

    // check if base case: an array
    unsigned i = 1;
    int iBrackDiff = 1;
    if (vec.at(0).data == "[") { 
        while (iBrackDiff != 0) {
            if (vec[i].data == "[") {
                iBrackDiff++;
            }
            else if (vec[i].data == "]") {
                iBrackDiff--;
            }

            if (i == vec.size() - 1) {
                break;
            }
            i++;
        }
        if (i == vec.size() - 1 || (i == vec.size() - 2 && vec.at(i + 1).type == "end")) { // BASE CASE: entire vec is single array, can still have no ]
            shared_ptr<AST2::Node> arrayNode(new AST2::Node);
            arrayNode->type = "array";
            arrayNode->leftChild = nullptr;
            arrayNode->rightChild = nullptr;
        
            unsigned j = 1;
            for (; j < vec.size(); j++) { // runs for each comma seperated element
                if (j == vec.size() - 1 || (j == vec.size() - 2 && vec.at(j + 1).type == "end")) { // at last index (not including end)
                    break;
                }
                vector<token> subVec;
                while (vec.at(j).data != ",") {
                    if (j == vec.size() - 1 || (j == vec.size() - 2 && vec.at(j + 1).type == "end")) { // at last index (not including end)
                        if (vec.at(j).data != "]") {
                            subVec.push_back(vec.at(j));
                        }
                        break;
                    }
                    subVec.push_back(vec.at(j));
                    j++;
                }
                // j at the comma or last element

                shared_ptr<AST2::Node> nodeElement(new AST2::Node);
                token emptyToken;
                nodeElement = build(subVec, emptyToken);
                arrayNode->array.push_back(nodeElement);
            }
            // j must be at last index of vec (not end token)

            if (iBrackDiff != 0) { // no closing bracket
                token errorToken = vec.at(i); // assumes there is end token
                error noEndBrack (errorToken.data, errorToken.row, errorToken.column, 2);
                throw noEndBrack;
            }

            return arrayNode;
        }
    }
    
    
    // case if argument is inside ()
    int paramCounter = 0;
    if (vec.at(0).data == "(") {
        unsigned i = 1; // go past parenthesis
        
        int parenthDiff = 1;

        while (parenthDiff != 0) {
            if (vec.at(i).type == "lParenth") {
                parenthDiff++;
            }
            else if (vec.at(i).type == "rParenth") {
                parenthDiff--;
            }
            if (i == vec.size() - 1 || parenthDiff == 0) { // break if i on last index or if parenthDiff is 0
                break;
            }
            if (vec.at(i).data != ")" && vec.at(i).data != "(") {
                paramCounter++;
            }
            i++;
        }
        // index at closing parenth or end of vector
        if (paramCounter < 1) {
            error emptyParenth(vec.at(i).data, vec.at(i).row, vec.at(i).column, 2);
            // cout << "test8" << endl;
            throw emptyParenth;
        }

        if ((vec.size() - 1) > i) { // more indexes past i, check if next index is end
            if (vec.at(i + 1).type == "end") {
                if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq" || vec.at(i - 1).type == "eqIneq" || vec.at(i - 1).type == "logicOp") {
                    token errorToken = vec.at(i);
                    error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                    // cout << "test9" << endl;
                    throw parenthNumEnd;
                }
                vec.erase(vec.begin() + i); // NOTE: have to erase end first
                vec.erase(vec.begin());
            }
        }
        else { // no indexes past i
            if (vec.at(i).type != "rParenth") {
                token errorToken = vec.at(i);
                error noClosingParenth(errorToken.data, errorToken.row, errorToken.column, 2);
                // cout << "test10" << endl;
                throw noClosingParenth;
            }
            if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq" || vec.at(i - 1).type == "eqIneq" || vec.at(i - 1).type == "logicOp") {
                token errorToken = vec.at(i - 1);
                error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                // cout << "test11" << endl;
                throw parenthNumEnd;
            }
            vec.pop_back();
            vec.erase(vec.begin());
        }
    }

    if (paramCounter == 1) {
        token emptyToken;
        return build(vec, emptyToken);
    }

    // we have an expresion of at least 1 operation & stripped of ()
    int lowestPrecedenceI = precedence(vec);
    
    shared_ptr<AST2::Node> oper(new AST2::Node);
    oper->data = vec.at(lowestPrecedenceI).data;
    oper->type = vec.at(lowestPrecedenceI).type;

    vector<token> leftVec;
    for (int j = 0; j < lowestPrecedenceI; j++) {
        leftVec.push_back(vec[j]);
    }
    if (leftVec.size() == 0) {
        token errorToken = vec.at(lowestPrecedenceI);
        error invalidOp(errorToken.data, errorToken.row, errorToken.column, 2);
        // cout << "test12" << endl;
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
        // cout << "test13" << endl;
        throw invalidOp;
    }

    oper->rightChild = build(rightVec, vec.at(lowestPrecedenceI));
    
    return oper;
}

void printInfix2(shared_ptr<AST2::Node> &someNode) {
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
    else if (someNode->type == "var" || someNode->type == "bool" || someNode->type == "null") {
        cout << someNode->data;
    }
    else if (someNode->type == "array") {
        cout << "[";
    
        int i = 0;
        for (; i < int(someNode->array.size() - 1); i++) {
            printInfix2(someNode->array.at(i));
            cout << ", ";
        }
        if (someNode->array.size() != 0) {
            printInfix2(someNode->array.at(i));
        }  
        cout << "]";
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

boolNum evaluate(shared_ptr<AST2::Node> &root, vector<variable> &variables){ 
    if (root->leftChild == nullptr && root->rightChild == nullptr) { // BASE CASE: when data is number, variable, bool, null, or array
        if (root->type == "var") { // if its a var
            bool assigned = false;
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
            if (!assigned) {
                error unassigned;
                unassigned.code = 3;
                unassigned.data = root->data;
                // cout << "test14" << endl;
                throw(unassigned); 
            }
        }
        else if (root->type == "bool") { // if its a bool
            boolNum boolVal(0, stob(root->data), "bool");
            return boolVal;
        }
        else if (root->type == "num") { // its a num
            boolNum numVal(stod(root->data), 0, "num");
            return numVal;
        }
        else if (root->type == "null") { // its a null
            boolNum nullVal(0, false, "null", "null");
            return nullVal;
        }
        else if (root->type == "array") { // else its an array
            boolNum result;
            result.mType = "array";
            for (unsigned i = 0; i < root->array.size(); i++) { // for each element at index i in the array
                Value someValue;
                boolNum arrayVal = evaluate(root->array.at(i), variables);
                if (arrayVal.mType == "bool") {
                    someValue = arrayVal.mBool;
                    result.mArray->push_back(someValue);
                }
                else if (arrayVal.mType == "num") {
                    someValue = arrayVal.mNum;
                    result.mArray->push_back(someValue);
                }
                else if (arrayVal.mType == "null") {
                    someValue = arrayVal.mNull;
                    result.mArray->push_back(someValue);
                }
                else if (arrayVal.mType == "array") {
                    someValue = (evaluate(root->array.at(i), variables)).mArray;
                    result.mArray->push_back(someValue);
                }
            }
            return result;
        }
    }

    // must be an operator
    if (root->data == "=") {
        if (root->leftChild->type != "var") {
            error invalidAssignee;
            invalidAssignee.code = 5;
            // cout << "test15" << endl;
            throw(invalidAssignee);
        }
        boolNum result;
        result = evaluate(root->rightChild, variables);
        if (result.mType == "bool") { // assignment to bool e.g. x = true
            variable var(root->leftChild->data, 0, result.mBool, "bool");
            bool update = false;
            for (int i = 0; i < int(variables.size()); i++) {
                if (variables[i].name == var.name) {
                    variables[i].type = "bool";
                    variables[i].boolValue = result.mBool;
                    update = true;
                }
            }
            if (!update) {
                variables.push_back(var);
            }
        }
        else { // else assignment to num e.g. x = 12;
            variable var(root->leftChild->data, result.mNum, 0, "num");
            bool update = false;
            for (int i = 0; i < int(variables.size()); i++) {
                if (variables[i].name == var.name) {
                    variables[i].type = "num";
                    variables[i].numValue = result.mNum;
                    update = true;
                }
            }
            if (!update) {
                variables.push_back(var);
            }
        }
        return result;
    }
    else if (root->type == "op") {
        if (evaluate(root->leftChild, variables).mType != "num" || evaluate(root->rightChild, variables).mType != "num") {
            error invalidReturn;
            invalidReturn.code = 4;
            // cout << "test16" << endl;
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
                // cout << "test17" << endl;
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
                // cout << "test18" << endl;
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
            // cout << "test19" << endl;
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
    else if (root->type == "eqIneq") {
        if (evaluate(root->leftChild, variables).mType == "bool") { // left child is bool
            if (evaluate(root->rightChild, variables).mType != "bool") {
                if (root->data == "==") {
                    boolNum result(0, false, "bool");
                    return result;
                }
                if (root->data == "!=") {
                    boolNum result(0, true, "bool");
                    return result;
                }
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
                if (root->data == "==") {
                    boolNum result(0, false, "bool");
                    return result;
                }
                if (root->data == "!=") {
                    boolNum result(0, true, "bool");
                    return result;
                }
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

void arrayPrinter(shared_ptr<std::vector<Value>> array) { // helper function to print arrays after evaluate
    cout << "[";

    int i = 0;
    for (; i < int(array->size() - 1); i++) {
        if (holds_alternative<double>(array->at(i))) {
            cout << get<double>(array->at(i)) << ", ";
        }
        else if (holds_alternative<bool>(array->at(i))) { // need this or else it will print out "0" or "1"
            if (get<bool>(array->at(i)) == true) {
                cout << "true ,";
            }
            else {
                cout << "false, ";
            }
        }
        else if (holds_alternative<string>(array->at(i))) {
            cout << "null ,";
        }
        else {
            arrayPrinter(get<shared_ptr<vector<Value>>>(array->at(i)));
            cout << ", ";
        }
    }
    if (array->size() != 0) {
        if (holds_alternative<double>(array->at(i))) {
        cout << get<double>(array->at(i));
        }
        else if (holds_alternative<bool>(array->at(i))) {
            if (get<bool>(array->at(i)) == true) { // need this or else it will print out "0" or "1"
                cout << "true";
            }
            else {
                cout << "false";
            }
        }
        else if (holds_alternative<string>(array->at(i))) {
            cout << "null";
        }
        else {
            arrayPrinter(get<shared_ptr<vector<Value>>>(array->at(i)));
        }
    }  
    cout << "]";
}