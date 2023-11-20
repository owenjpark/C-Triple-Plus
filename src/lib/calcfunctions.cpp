#include "calc.h"
#include "lex.h"

#include <string.h>
#include <cmath>

AST2::AST2() {
    root = nullptr;
};

AST2::~AST2() {
}

double precedence(vector<token> vec) {
    // TODO: delete this
    // cout << "getting precedence of vec: ";
    // for (unsigned i = 0; i < vec.size(); i++) {
    //     cout << vec.at(i).data << " ";
    // }
    // cout << endl;

    // PRESCEDENCE AS FOLLOWS
    // "="                  0
    // "|"                  1 
    // "^"                  2
    // "&"                  3
    // "==" "!="            4
    // "<" "<=" ">" ">="    5
    // "+" "-"              6
    // "*" "/" "%"          7
    // lookUp               8
    // "(" ")" "[" "]       9
    // #, var, bool, null   10

   int currLowestRating = 11; // initialize to any value above 10 (higest precedence)
   double leastPrecedenceIndex;
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
                // i is at either last token or token after ]
                if (vec.at(i).data == "[") { // potential start of array lookup
                    currPrecedence = 8;
                    i--;
                }
                else {
                    currPrecedence = 9;
                    i--;
                }
                
            }
        }
        else if (vec[i].data == "(" || vec[i].data == ")") { // ideally shouldn't happen, should be caught by base cases
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
            currPrecedence = 9;
        }
        else { // else its a number, variable, bool, or null; ideally shouldn't happen, should be caught by base cases
            if (i < vec.size() - 1 && vec[i + 1].data == "[") {
                currPrecedence = 8;
            }
            else {
                currPrecedence = 10;
            }
        }

        if (currPrecedence <= currLowestRating) {
            if (currLowestRating == 0 && currPrecedence == 0) {
                // do nothing if seeing another "=" bc it's right associative
            }
            else {
                currLowestRating = currPrecedence;
                if (currPrecedence == 8) { // special case for lookUp operator, in between [1, 2]here[0]
                    leastPrecedenceIndex = i + 0.5;
                }
                else {
                    leastPrecedenceIndex = i;
                }
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
    if (currLowestRating > 8) { // no operators
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
            // cout << "test2" << endl;
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
            // cout << "test3" << endl;
            throw noOperator;
        }

        // neither of two above cases, error on 2nd index
        token errorToken = vec.at(1);
        error noOperator(errorToken.data, errorToken.row, errorToken.column, 2);
        // cout << "test4" << endl;
        throw noOperator;
    }
    if (vec.at(leastPrecedenceIndex).data == ")") {
        token errorToken = vec.at(leastPrecedenceIndex);
        error rParenthError(errorToken.data, errorToken.row, errorToken.column, 2);
        // cout << "test5" << endl;
        throw rParenthError;
    }
    
    return leastPrecedenceIndex;
}

shared_ptr<AST2::Node> build(vector<token> vec, token parentToken) {
    // TODO: delete
    // cout << "building with vec: ";
    // for (unsigned i = 0; i < vec.size(); i++) {
    //     cout << vec.at(i).data << " ";
    // }
    // cout << endl;

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
            // cout << "test6" << endl;
            throw(empty);
        }
        else if (vec.at(0).type == "lParenth" || vec.at(0).type == "lSquareBracket") { // SPECIAL CASE: "(" error has to be column 2
            token errorToken = vec.at(1);
            error noFirstOperand (errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test7" << endl;
            throw noFirstOperand;
        }
        else if (vec.at(0).type == "condition" || vec.at(0).type == "print") { 
            token errorToken = vec.at(0);
            error noStatement (errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test8" << endl;
            throw noStatement;
        }
        else { // else its not num, variable, bool, or null
            token errorToken = vec.at(0);
            error noFirstOperand(errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test9" << endl;
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
            // i is at index after [ or at last index (no closing brack)
            shared_ptr<AST2::Node> arrayNode(new AST2::Node);
            arrayNode->type = "array";
            arrayNode->leftChild = nullptr;
            arrayNode->rightChild = nullptr;
        
            unsigned j = 1;
            int jBrackDiff = 0;
            if (vec.at(j).data == "[") {
                jBrackDiff = 1;
            }
            for (; j < vec.size(); j++) { // runs for each comma seperated element
                if (j == vec.size() - 1 || (j == vec.size() - 2 && vec.at(j + 1).type == "end")) { // at last index (not including end) e.g. [12, 
                    if (vec.at(j).data == ",") {
                        token errorToken = vec.at(j);
                        error onlyComma (errorToken.data, errorToken.row, errorToken.column, 2);
                        // cout << "test9.3" << endl;
                        throw onlyComma;
                    }
                    break;
                }
                vector<token> subVec;
                while (true) { // get one element of vector
                    if (vec.at(j).data == "," && jBrackDiff == 0 ){
                        break;
                    }
                    if (j == vec.size() - 1 || (j == vec.size() - 2 && vec.at(j + 1).type == "end")) { // at last index (not including end) e.g. [12, 0
                        if (vec.at(j).data != "]") {
                            subVec.push_back(vec.at(j));
                        }
                        break;
                    }

                    if (vec.at(j).data == "[") {
                        jBrackDiff++;
                    }
                    else if (vec.at(j).data == "]") {
                        jBrackDiff--;
                    }

                    subVec.push_back(vec.at(j));
                    j++;
                }
                // j at the comma or last element
                if (subVec.size() == 0) {
                    token errorToken = vec.at(j); // assumes there is end token TODO: not always true eg: [[2, 
                    error emptyElement (errorToken.data, errorToken.row, errorToken.column, 2);
                    // cout << "test9.4" << endl;
                    throw emptyElement;
                }

                shared_ptr<AST2::Node> nodeElement(new AST2::Node);
                token emptyToken;
                nodeElement = build(subVec, emptyToken);
                arrayNode->array.push_back(nodeElement);
            }
            // j must be at last index of vec (not end token)
            if (iBrackDiff != 0) { // no closing bracket (check after checking elements)
                token errorToken = vec.at(i); // assumes there is end token TODO: not always true eg: [[2, 
                error noEndBrack (errorToken.data, errorToken.row, errorToken.column, 2);
                // cout << "test9.5" << endl;
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
            // cout << "test10" << endl;
            throw emptyParenth;
        }

        if ((vec.size() - 1) > i) { // more indexes past i, check if next index is end
            if (vec.at(i + 1).type == "end") {
                if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq" || vec.at(i - 1).type == "eqIneq" || vec.at(i - 1).type == "logicOp") {
                    token errorToken = vec.at(i);
                    error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                    // cout << "test11" << endl;
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
                // cout << "test12" << endl;
                throw noClosingParenth;
            }
            if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq" || vec.at(i - 1).type == "eqIneq" || vec.at(i - 1).type == "logicOp") {
                token errorToken = vec.at(i - 1);
                error parenthNumEnd(errorToken.data, errorToken.row, errorToken.column, 2);
                // cout << "test13" << endl;
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
    double lowestPrecedenceI = precedence(vec);
    if (fmod(lowestPrecedenceI, 1) == 0) { // if not array lookup
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
            // cout << "test14" << endl;
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
            // cout << "test15" << endl;
            throw invalidOp;
        }
        oper->rightChild = build(rightVec, vec.at(lowestPrecedenceI));
        
        return oper;
    }
    else { // special case for lookup; lowestPrecedenceI in between two indexes, ends in ".5"
        shared_ptr<AST2::Node> oper(new AST2::Node);
        oper->data = "lookUp";
        oper->type = "lookUp";

        vector<token> leftVec;
        for (double j = 0; j < lowestPrecedenceI; j++) {
            leftVec.push_back(vec[j]);
        }
        if (leftVec.size() == 0) {
            token errorToken = vec.at(lowestPrecedenceI);
            error invalidOp(errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test16" << endl;
            throw invalidOp;
        }
        oper->leftChild = build(leftVec, vec.at(lowestPrecedenceI));
        
        vector<token> rightVec;
        double i = lowestPrecedenceI + 1.5;
        while (i != vec.size() - 1) { // assumes that we only have lookup in vec b/c highest precedence; stops at ]
            if (i == vec.size() - 2 && vec.at(i + 1).type == "end") {
                break;
            }
            rightVec.push_back(vec[i]);
            i++;
        }
        if (rightVec.size() == 0) {
            // TODO: delete print debugging
            // for (unsigned i = 0; i < vec.size(); i++) {
            //     cout << vec.at(i).data << " ";
            // }
            // cout << endl;
            token errorToken = parentToken;
            error invalidOp(errorToken.data, errorToken.row, errorToken.column, 2);
            // cout << "test17.2" << endl;
            throw invalidOp;
        }
        oper->rightChild = build(rightVec, vec.at(lowestPrecedenceI));
        
        return oper;
    }
    
}

void printInfix2(shared_ptr<AST2::Node> &someNode) {
    if (someNode->type == "op" || someNode->type == "eq" || someNode->type == "eqIneq" || someNode->type == "logicOp") {
        cout << "(" ;
    }

    if (someNode->type == "lookUp") {
        printInfix2(someNode->leftChild);
        cout << "[";
        printInfix2(someNode->rightChild);
        cout << "]";
    }
    else if (someNode->leftChild != nullptr && someNode->rightChild != nullptr) {
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
    else if (someNode->type == "lookUp") {
        // print nothing
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
                    else if (variables[i].type == "num") { 
                        boolNum varValue(variables[i].numValue, 0, "num");
                        return varValue;
                    }
                    else if (variables[i].type == "array") {
                        boolNum varValue;
                        varValue.mType = "array";
                        varValue.mArray = variables[i].arrayValue;
                        return varValue;
                    }
                    // TODO: variable assigned to null?
                } 
            } 
            if (!assigned) {
                error unassigned;
                unassigned.code = 3;
                unassigned.data = root->data;
                // cout << "test16" << endl;
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
    if (root->data == "=") { // if assignment
        if (root->leftChild->type != "var" && root->leftChild->type != "lookUp") {
            error invalidAssignee;
            invalidAssignee.code = 5;
            // cout << "test17.1" << endl;
            throw(invalidAssignee);
        }
        if (root->leftChild->type == "var") { // regular assignment
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
            else if (result.mType == "num") { // else if assignment to num e.g. x = 12;
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
            else if (result.mType == "array") { // else if assignment to array
                variable var;
                var.name = root->leftChild->data;
                var.type = "array";
                var.arrayValue = result.mArray;
                bool update = false;
                for (int i = 0; i < int(variables.size()); i++) {
                    if (variables[i].name == var.name) {
                        variables[i].type = "array";
                        variables[i].arrayValue = result.mArray;
                        update = true;
                    }
                }
                if (!update) {
                    variables.push_back(var);
                }
            }
            return result;
        }
        else if (root->leftChild->type == "lookUp") { // array assignment
            boolNum left = evaluate(root->leftChild->leftChild, variables);
            boolNum right = evaluate(root->leftChild->rightChild, variables);
            if (left.mType != "array") {
                error notArray;
                notArray.code = 6;
                // cout << "test17.4" << endl;
                throw(notArray);
            }
            if (right.mType != "num") {
                error indexNotNum;
                indexNotNum.code = 7;
                // cout << "test17.5" << endl;
                throw(indexNotNum);
            }
            if (right.mType == "num") { // check if integer
                if (fmod(right.mNum, 1) != 0) {
                    error indexNotInt;
                    indexNotInt.code = 9;
                    // cout << "test17.5" << endl;
                    throw(indexNotInt);
                }
            }
            if (right.mNum > left.mArray->size() - 1 || right.mNum < 0) {
                error indexOutOfBounds;
                indexOutOfBounds.code = 8;
                // cout << "test17.6" << endl;
                throw(indexOutOfBounds);
            }

            boolNum reassignBoolNum = evaluate(root->rightChild, variables);
            Value reassignVal;
            if (reassignBoolNum.mType == "array") {
                reassignVal = reassignBoolNum.mArray;
            }
            else if (reassignBoolNum.mType == "num") {
                reassignVal = reassignBoolNum.mNum;
            }
            else if (reassignBoolNum.mType == "bool") {
                reassignVal = reassignBoolNum.mBool;
            }
            else if (reassignBoolNum.mType == "null") {
                reassignVal = reassignBoolNum.mNull;
            }
            shared_ptr<vector<Value>> reassignVec = left.mArray;
            (*reassignVec)[right.mNum] = reassignVal;
            
            return reassignBoolNum;
        }
    }
    if (root->type == "lookUp") { // lookUp
        boolNum left = evaluate(root->leftChild, variables);
        boolNum right = evaluate(root->rightChild, variables);
        if (left.mType != "array") {
            error notArray;
            notArray.code = 6;
            // cout << "test17.4" << endl;
            throw(notArray);
        }
        if (right.mType != "num") {
            error indexNotNum;
            indexNotNum.code = 7;
            // cout << "test17.5" << endl;
            throw(indexNotNum);
        }
        if (right.mType == "num") { // check if integer
            if (fmod(right.mNum, 1) != 0) {
                error indexNotInt;
                indexNotInt.code = 9;
                // cout << "test17.5" << endl;
                throw(indexNotInt);
            }
        }
        if (right.mNum > left.mArray->size() - 1 || right.mNum < 0) {
            error indexOutOfBounds;
            indexOutOfBounds.code = 8;
            // cout << "test17.6" << endl;
            throw(indexOutOfBounds);
        }
        // we can proceed to access the element at the index
        boolNum result;
        if (holds_alternative<double>(left.mArray->at(right.mNum))) {
            result.mType = "num";
            result.mNum = get<double>(left.mArray->at(right.mNum));
        }
        else if (holds_alternative<bool>(left.mArray->at(right.mNum))){
            result.mType = "bool";
            result.mNum = get<bool>(left.mArray->at(right.mNum));
        }
        else if (holds_alternative<string>(left.mArray->at(right.mNum))) {
            result.mType = "null";
            result.mNull = get<string>(left.mArray->at(right.mNum));
        }
        else { // else it holds a array
            result.mType = "array";
            result.mArray = get<shared_ptr<vector<Value>>>(right.mArray->at(right.mNum));
        }
        return result;
    }
    else if (root->type == "op") {
        if (evaluate(root->leftChild, variables).mType != "num" || evaluate(root->rightChild, variables).mType != "num") {
            error invalidReturn;
            invalidReturn.code = 4;
            // cout << "test18" << endl;
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
                // cout << "test19" << endl;
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
                // cout << "test20" << endl;
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
            // cout << "test21" << endl;
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
        else if (evaluate(root->leftChild, variables).mType == "num"){ // else left child is a num
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
        else { // else left child is an array
            if (evaluate(root->rightChild, variables).mType != "array") {
                if (root->data == "==") {
                    boolNum result(0, false, "bool");
                    return result;
                }
                if (root->data == "!=") {
                    boolNum result(0, true, "bool");
                    return result;
                }
            }
            
            
            boolNum result(0, true, "bool");
            shared_ptr<std::vector<Value>> leftArray = evaluate(root->leftChild, variables).mArray;
            shared_ptr<std::vector<Value>> rightArray = evaluate(root->rightChild, variables).mArray;
            if (leftArray->size() != rightArray->size()) {
                result.mBool = false;
            }
            for (unsigned i = 0; i < leftArray->size(); i++) {
                if (leftArray->at(i) != rightArray->at(i)) {
                    result.mBool = false;
                    break;
                }
            }
            if (root->data == "==") {
                // do nothing
            }
            else if (root->data == "!="){
                result.mBool = !result.mBool;
            }
            return result;
        }
        // TODO: null comparison?
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
                cout << "true, ";
            }
            else {
                cout << "false, ";
            }
        }
        else if (holds_alternative<string>(array->at(i))) {
            cout << "null, ";
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