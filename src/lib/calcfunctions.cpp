
#include "calc.h"
#include "lex.h"
#include "scrypt.h"

#include <string.h>
#include <cmath>
#include <string>

int parenthChecker(unsigned i, vector<token> tokenVec) { // helper function for expressionChecker; checks expressions in parenthesis; starts at i after "("; returns i at ")" or "END"
    int parenthDiff = 1;
    unsigned startI = i;
    while (i < tokenVec.size() - 1) {
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
        if (parenthDiff == 0) { 
            break;
        }

        i++;
    }
    unsigned endI = i;
    // endI at "END" or ")"

    if (startI == endI) { // if passing in ()
        error emptyParenth(tokenVec.at(endI).data, 1, tokenVec.at(endI).column, 2);
        // cout << "test1" << endl;
        throw emptyParenth;
    }
    expressionChecker(startI, endI, tokenVec);
    if (parenthDiff != 0) {
        error noClosingParenth(tokenVec.at(endI).data, 1, tokenVec.at(endI).column, 2);
        // cout << "test2" << endl;
        throw noClosingParenth;
    }

    return endI;
}

int brackChecker(unsigned i, vector<token> tokenVec) { // helper function for expressionChecker; checks expressions in square brackets; starts at i after "["; returns i at "]" or "END"
    int brackDiff = 1;
    unsigned startI = i;
    while (i < tokenVec.size() - 1) {
        if (tokenVec.at(i).type == "lSquareBracket") {
            brackDiff++;
        }
        if (tokenVec.at(i).type == "rSquareBracket") {
            brackDiff--;
        }
        if (brackDiff == 0) { 
            break;
        }

        i++;
    }
    unsigned endI = i;
    // endI at end token or "]"

    unsigned j = startI;
    int commaBrackDiff = 0;
    int commaParenthDiff = 0;
    for (;j < endI; j++) {
        if (tokenVec.at(j).data == "[") {
            commaBrackDiff++;
        }
        else if (tokenVec.at(j).data == "]") {
            commaBrackDiff--;
        }
        else if (tokenVec.at(j).data == "(") {
            commaParenthDiff++;
        }
        else if (tokenVec.at(j).data == ")") {
            commaParenthDiff--;
        }

        if (tokenVec.at(j).type == "comma" && commaBrackDiff == 0 && commaParenthDiff == 0) {
            expressionChecker(startI, j, tokenVec);
            startI = j + 1;
        }
    }
    if (tokenVec.at(endI - 1).type == "comma") { // if ends with comma e.g. [1,]
        error commaEnd(tokenVec.at(j).data, 1, tokenVec.at(j).column, 2);
        // cout << "test3" << endl;
        throw commaEnd;
    }
    if (startI != endI) { // check last comma seperated element only if brackets not empty e.g. []
        expressionChecker(startI, j, tokenVec);
    }
    if (brackDiff != 0) {
        error noCLosingBrack(tokenVec.at(endI).data, 1, tokenVec.at(endI).column, 2);
        // cout << "test4" << endl;
        throw noCLosingBrack;
    }

    return endI;
}

int paramChecker(unsigned i, vector<token> tokenVec) { // helper function for expressionChecker; checks parameters for function calls; starts at i after "("; returns i at "]" or "END"
    int parenthDiff = 1;
    unsigned startI = i;
    while (i < tokenVec.size() - 1) {
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
        if (parenthDiff == 0) { 
            break;
        }

        i++;
    }
    unsigned endI = i;
    // endI at end token or "]"

    unsigned j = startI;
    int commaBrackDiff = 0;
    int commaParenthDiff = 0;
    for (;j < endI; j++) { // need to skip inner commas
        if (tokenVec.at(j).data == "[") {
            commaBrackDiff++;
        }
        else if (tokenVec.at(j).data == "]") {
            commaBrackDiff--;
        }
        else if (tokenVec.at(j).data == "(") {
            commaParenthDiff++;
        }
        else if (tokenVec.at(j).data == ")") {
            commaParenthDiff--;
        }

        if (tokenVec.at(j).type == "comma" && commaBrackDiff == 0 && commaParenthDiff == 0) {
            expressionChecker(startI, j, tokenVec);
            startI = j + 1;
        }
    }
    if (tokenVec.at(endI - 1).type == "comma") { // if ends with comma e.g. [1,]
        error commaEnd(tokenVec.at(j).data, 1, tokenVec.at(j).column, 2);
        // cout << "test5" << endl;
        throw commaEnd;
    }
    if (startI != endI) { // check last comma seperated element only if brackets not empty e.g. []
        expressionChecker(startI, j, tokenVec);
    }
    if (parenthDiff != 0) {
        error noCLosingParenth(tokenVec.at(endI).data, 1, tokenVec.at(endI).column, 2);
        // cout << "test6" << endl;
        throw noCLosingParenth;
    }

    return endI;
}

void expressionChecker(unsigned startIndex, unsigned endIndex, vector<token> tokenVec) {
    if (tokenVec.at(startIndex).type != "num" && tokenVec.at(startIndex).type != "bool" &&  tokenVec.at(startIndex).type != "var" && tokenVec.at(startIndex).type != "null" && tokenVec.at(startIndex).type != "lParenth" && tokenVec.at(startIndex).type != "lSquareBracket" && tokenVec.at(startIndex).type != "name") { // doesn't start with value
        error invalidExpressStart(tokenVec.at(startIndex).data, 1, tokenVec.at(startIndex).column, 2);
        // cout << "test7" << endl;
        throw invalidExpressStart;
    }
    // at least 1 element

    for (unsigned i = startIndex; i < endIndex; i++) { // stops at i before "END"
        if (tokenVec.at(i).type == "lParenth") {
            i++; // skip "("
            int endParenthIndex = parenthChecker(i, tokenVec);
            i = endParenthIndex;
            // i at ")"
            if (i < tokenVec.size() - 1 && i + 1 != endIndex) { // prevents seg fault and prevents checking next if it's the last token ("END", ")", or "]")
                if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "eqIneq" && tokenVec.at(i + 1).type != "logicOp" && tokenVec.at(i + 1).type != "end") { 
                    error nextNotOp(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                    // cout << "test8" << endl;
                    throw nextNotOp;
                }
            }
        }
        if (tokenVec.at(i).type == "lSquareBracket") {
            i++; // skip "["
            int endParenthIndex = brackChecker(i, tokenVec);
            i = endParenthIndex;
            // i at ")"
            if (i < tokenVec.size() - 1 && i + 1 != endIndex) { // prevents seg fault and prevents checking next if it's the last token ("END", ")", or "]")
                if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "eqIneq" && tokenVec.at(i + 1).type != "logicOp" && tokenVec.at(i + 1).type != "end" && tokenVec.at(i + 1).type != "lSquareBracket") { 
                    error nextNotOp(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                    // cout << "test9" << endl;
                    throw nextNotOp;
                }
            }
        }
        if (tokenVec.at(i).type == "num" || tokenVec.at(i).type == "bool" || tokenVec.at(i).type == "var" || tokenVec.at(i).type == "null" || tokenVec.at(i).type == "lParenth" || tokenVec.at(i).type == "rParenth") {
            if (i < tokenVec.size() - 1 && i + 1 != endIndex) { // prevents seg fault and prevents checking next if it's the last token ("END", ")", or "]")
                if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "eqIneq" && tokenVec.at(i + 1).type != "logicOp" && tokenVec.at(i + 1).type != "end" && tokenVec.at(i + 1).type != "lSquareBracket") { 
                    error nextNotOp(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                    // cout << "test10" << endl;
                    throw nextNotOp;
                }
            }
        }
        if (tokenVec.at(i).type == "name") { // NOTE: must have "(" as next token per how lexer works
            i++; // i at (
            if (i < tokenVec.size() - 1 && i + 1 != endIndex) { // prevents seg fault
                i++;
                int endParenthIndex = paramChecker(i, tokenVec);
                i = endParenthIndex;
                if (i < tokenVec.size() - 1 && i + 1 != endIndex) { // prevents seg fault and prevents checking next if it's the last token ("END", ")", or "]")
                    if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "eqIneq" && tokenVec.at(i + 1).type != "logicOp" && tokenVec.at(i + 1).type != "end") { 
                        error nextNotOp(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                        // cout << "test11" << endl;
                        throw nextNotOp;
                    }
                }
            }
            else { // case where its "someFunction("
                error invalidCall(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                // cout << "test12" << endl;
                throw invalidCall;
            }
        }
        if (tokenVec.at(i).type == "op" || tokenVec.at(i).type == "eq" || tokenVec.at(i).type == "eqIneq" || tokenVec.at(i).type == "logicOp") {
            if (i < tokenVec.size() - 1) { // prevents seg fault
                if (tokenVec.at(i + 1).type != "num" && tokenVec.at(i + 1).type != "bool" && tokenVec.at(i + 1).type != "var" && tokenVec.at(i + 1).type != "null" && tokenVec.at(i + 1).type != "lParenth" && tokenVec.at(i + 1).type != "lSquareBracket" && tokenVec.at(i + 1).type != "name") {
                    error nextNotNum(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                    // cout << "test13" << endl;
                    throw nextNotNum; // NOTE: this will catch end token error e.g 1 + end
                }
            }
        }
    }
}

double precedence(vector<token> vec) {
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
   int currPrecedence;
   double leastPrecedenceIndex;

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
            if (i < vec.size() - 1) { // prevents seg fault
                i++;
                int brackDiff = 1;
                while (brackDiff != 0 && i != vec.size() - 1) { // goes to index after "]" or last index
                    if (vec[i].data == "[") {
                        brackDiff++;
                    }
                    else if (vec[i].data == "]") {
                        brackDiff--;
                    }
                    i++;
                }
                // index is at either last token or token after ]
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
                if (currPrecedence == 8) { // special case for lookUp operator, leastPrecedenceIndex set between array and index
                    leastPrecedenceIndex = i + 0.5;
                }
                else {
                    leastPrecedenceIndex = i;
                }
            }
        }
        i++;
    }

    return leastPrecedenceIndex;
}

shared_ptr<AST2::Node> build(vector<token> vec) {
    cout << vec.size() << endl;
    if (vec.size() == 1 || (vec.size() == 2 && vec.at(1).type == "end")) {
        if (vec.at(0).type == "num" || vec.at(0).type == "var" || vec.at(0).type == "bool" || vec.at(0).type == "null") { // BASE CASE: vec has only num, variable, bool, or null
            shared_ptr<AST2::Node> node(new AST2::Node);
            node->data = vec.at(0).data;
            node->type = vec.at(0).type;
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
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

                shared_ptr<AST2::Node> nodeElement(new AST2::Node);
                nodeElement = build(subVec);
                arrayNode->array.push_back(nodeElement);
            }
            // j must be at last index of vec (not end token)

            return arrayNode;
        }
    }
    
    // case if argument is inside ()
    int paramCounter = 0;
    if (vec.size() > 0 && vec.at(0).data == "(") {
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

        if ((vec.size() - 1) > i) { // more indexes past i, check if next index is end
            if (vec.at(i + 1).type == "end") {
                vec.erase(vec.begin() + i); // NOTE: have to erase end first
                vec.erase(vec.begin());
            }
        }
        else { // no indexes past i
            vec.pop_back();
            vec.erase(vec.begin());
        }
    }
    if (paramCounter == 1) {
        return build(vec);
    }

    // we have an expresion of at least 1 operation & stripped of ()
    
    // Base case for if an expression that has a function call 
    if (vec.size() > 0 && vec.at(0).type == "name") { // each parameter in "array", and name of call in data
        shared_ptr<AST2::Node> funcCall(new AST2::Node);
        funcCall->type = "funcCall";
        funcCall->data = vec.at(0).data;
        funcCall->leftChild = nullptr;
        funcCall->rightChild = nullptr;

        unsigned j = 2;
            int jParenthDiff = 0;
            if (vec.at(j).data == "(") {
                jParenthDiff = 1;
            }
            for (; j < vec.size(); j++) { // runs for each comma seperated element
                if (j == vec.size() - 1 || (j == vec.size() - 2 && vec.at(j + 1).type == "end")) { // at last index (not including end) e.g. [12, 
                    break;
                }
                vector<token> subVec;
                while (true) { // get one element of vector
                    if (vec.at(j).data == "," && jParenthDiff == 0 ){
                        break;
                    }
                    if (j == vec.size() - 1 || (j == vec.size() - 2 && vec.at(j + 1).type == "end")) { // at last index (not including end) e.g. [12, 0
                        if (vec.at(j).data != ")") {
                            subVec.push_back(vec.at(j));
                        }
                        break;
                    }

                    if (vec.at(j).data == "[") {
                        jParenthDiff++;
                    }
                    else if (vec.at(j).data == "]") {
                        jParenthDiff--;
                    }

                    subVec.push_back(vec.at(j));
                    j++;
                }
                // j at the comma or last element

                shared_ptr<AST2::Node> nodeElement(new AST2::Node);
                nodeElement = build(subVec);
                funcCall->array.push_back(nodeElement);
            }
            // j must be at last index of vec (not end token)

            return funcCall;
    }

    double lowestPrecedenceI = precedence(vec);
    if (fmod(lowestPrecedenceI, 1) == 0) { // if not array lookup
        shared_ptr<AST2::Node> oper(new AST2::Node);
        oper->data = vec.at(lowestPrecedenceI).data;
        oper->type = vec.at(lowestPrecedenceI).type;

        vector<token> leftVec;
        for (int j = 0; j < lowestPrecedenceI; j++) {
            leftVec.push_back(vec[j]);
        }

        oper->leftChild = build(leftVec);
        
        vector<token> rightVec;
        for (unsigned i = lowestPrecedenceI + 1; i < vec.size(); i++) {
            rightVec.push_back(vec[i]);
        }
        oper->rightChild = build(rightVec);
        
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
        oper->leftChild = build(leftVec);
        
        vector<token> rightVec;
        double i = lowestPrecedenceI + 1.5;
        while (i != vec.size() - 1) { // assumes that we only have lookup in vec b/c highest precedence; stops at ]
            if (i == vec.size() - 2 && vec.at(i + 1).type == "end") {
                break;
            }
            rightVec.push_back(vec[i]);
            i++;
        }
        oper->rightChild = build(rightVec);
        
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
    else if (someNode->type == "funcCall") {
        cout << someNode->data << "(";
    
        int i = 0;
        for (; i < int(someNode->array.size() - 1); i++) {
            printInfix2(someNode->array.at(i));
            cout << ", ";
        }
        if (someNode->array.size() != 0) {
            printInfix2(someNode->array.at(i));
        }

        cout << ")";
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
    // cout << "entering evaluate" << endl;
    if (root->leftChild == nullptr && root->rightChild == nullptr) { // BASE CASE: when data is number, variable, bool, null, or array
        if (root->type == "var") { // if its a var
            bool assigned = false;
            for (int i = 0; i < int(variables.size()); i++){ // iterate through variables exists
                if (variables[i].name == root->data) { // variable aklexists, reassign
                    assigned = true;
                    if (variables[i].type == "num") { 
                        boolNum varValue("num", variables[i].numValue, false);
                        // cout << "exiting evaluate" << endl;
                        return varValue;
                    }
                    else if (variables[i].type == "bool") {
                        boolNum varValue("bool", 0, variables[i].boolValue);
                        // cout << "exiting evaluate" << endl;
                        return varValue;
                    }
                    else if (variables[i].type == "null") {
                        boolNum varValue("null", 0, false);
                        // cout << "exiting evaluate" << endl;
                        return varValue;
                    }
                    else if (variables[i].type == "array") {
                        boolNum varValue;
                        varValue.mType = "array";
                        varValue.mArray = variables[i].arrayValue;
                        // cout << "exiting evaluate" << endl;
                        return varValue;
                    }
                } 
            } 
            if (!assigned) {
                error unassigned;
                unassigned.code = 3;
                unassigned.data = root->data;
                //cout << "error 13" << endl;
                throw(unassigned); 
            }
        }
        else if (root->type == "bool") { // if its a bool
            boolNum boolVal("bool", 0, stob(root->data));
            // cout << "exiting evaluate" << endl;
            return boolVal;
        }
        else if (root->type == "num") { // its a num
            boolNum numVal("num", stod(root->data), 0);
            // cout << "exiting evaluate" << endl;
            return numVal;
        }
        else if (root->type == "null") { // its a null
            boolNum nullVal("null", 0, false);
            // cout << "exiting evaluate" << endl;
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
                    someValue = arrayVal.mType;
                    result.mArray->push_back(someValue);
                }
                else if (arrayVal.mType == "array") {
                    someValue = (evaluate(root->array.at(i), variables)).mArray;
                    result.mArray->push_back(someValue);
                }
            }
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->type == "funcCall") {
            // TODO: len, pop, push?
            unsigned paramCounter = 0;
            for (unsigned i = 0; i < variables.size(); i++) {
                if (root->data == variables.at(i).name) {
                    
                    // name matches!
                    vector<variable> localLocalScope = vector<variable>(variables.at(i).funcVal.localScope);
                    for (unsigned j = 0; j < localLocalScope.size(); j++) { // assigning parameters
                        if (root->array.size() - 1 < paramCounter) { // we have too little parameters
                            error notArray;
                            notArray.code = 10;
                            throw(notArray);
                        }
                        if (localLocalScope.at(j).type == "parameter") {
                            boolNum parameterResult = evaluate(root->array.at(paramCounter), variables);
                            if (parameterResult.mType == "num") {
                                localLocalScope.at(j).type = "num";
                                localLocalScope.at(j).numValue = parameterResult.mNum;
                            }
                            else if (parameterResult.mType == "bool") {
                                localLocalScope.at(j).type = "bool";
                                localLocalScope.at(j).boolValue = parameterResult.mBool;
                            }
                            else if (parameterResult.mType == "array") {
                                localLocalScope.at(j).type = "array";
                                localLocalScope.at(j).arrayValue = parameterResult.mArray;
                            }
                            paramCounter++;
                        }
                    }
                    if (paramCounter != root->array.size()) {
                        error notArray;
                        notArray.code = 10;
                        throw(notArray);
                    }
                    Value resultVal= runProgram(variables.at(i).funcVal.statements, localLocalScope);
                    boolNum result;
                    if (holds_alternative<double>(resultVal)) {
                        result.mType = "num";
                        result.mNum = get<double>(resultVal);
                    }
                    else if (holds_alternative<bool>(resultVal)) {
                        result.mType = "bool";
                        result.mBool = get<bool>(resultVal);
                    }
                    else if (holds_alternative<string>(resultVal)) {
                        result.mType = "string";
                    }
                    else if (holds_alternative<shared_ptr<vector<Value>>>(resultVal)) {
                        result.mType = "array";
                        result.mArray = get<shared_ptr<vector<Value>>>(resultVal);
                    }
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
            }
        }
    }

    // not a base case, must be an operator
    if (root->data == "=") { // if assignment
        if (root->leftChild->type != "var" && root->leftChild->type != "lookUp") {
            error invalidAssignee;
            invalidAssignee.code = 5;
            //cout << "error 14" << endl;
            throw(invalidAssignee);
        }
        if (root->leftChild->type == "var") { // regular assignment
            boolNum result;
            result = evaluate(root->rightChild, variables);
            // cout << "kachow: " << root->rightChild->rightChild->data << endl;
            if (result.mType == "num") { // else if assignment to num e.g. x = 12;
                variable var("num", root->leftChild->data, result.mNum, 0);
                bool update = false;
                for (int i = 0; i < int(variables.size()); i++) { // check if variable exists, update it
                    if (variables[i].name == var.name) {
                        variables[i].type = "num";
                        variables[i].numValue = result.mNum;
                        update = true;
                    }
                }
                if (!update) { // variable doesn't exist push_back
                    variables.push_back(var);
                }
            }
            else if (result.mType == "bool") { // assignment to bool e.g. x = true
                variable var("bool", root->leftChild->data, 0, result.mBool);
                bool update = false;
                for (int i = 0; i < int(variables.size()); i++) { // check if variable exists, update it
                    if (variables[i].name == var.name) {
                        variables[i].type = "bool";
                        variables[i].boolValue = result.mBool;
                        update = true;
                    }
                }
                if (!update) { // variable doesn't exist push_back
                    variables.push_back(var);
                }
            }
            else if (result.mType == "null") { // assignment to null
                variable var("null", root->leftChild->data, 0, result.mBool);
                bool update = false;
                for (int i = 0; i < int(variables.size()); i++) { // check if variable exists, update it
                    if (variables[i].name == var.name) {
                        variables[i].type = "null";
                        update = true;
                    }
                }
                if (!update) { // variable doesn't exist push_back
                    variables.push_back(var);
                }
            }
            else if (result.mType == "array") { // else if assignment to array
                variable var;
                var.name = root->leftChild->data;
                var.type = "array";
                var.arrayValue = result.mArray;
                bool update = false;
                for (int i = 0; i < int(variables.size()); i++) { // check if variable exists, update it
                    if (variables[i].name == var.name) {
                        variables[i].type = "array";
                        variables[i].arrayValue = result.mArray;
                        update = true;
                    }
                }
                if (!update) { // variable doesn't exist push_back
                    variables.push_back(var);
                }
            }
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->leftChild->type == "lookUp") { // array assignment
            boolNum left = evaluate(root->leftChild->leftChild, variables);
            boolNum right = evaluate(root->leftChild->rightChild, variables);
            if (left.mType != "array") {
                error notArray;
                notArray.code = 6;
                throw(notArray);
            }
            if (right.mType != "num") {
                error indexNotNum;
                indexNotNum.code = 7;
                throw(indexNotNum);
            }
            if (right.mType == "num") { // check if integer
                if (fmod(right.mNum, 1) != 0) {
                    error indexNotInt;
                    indexNotInt.code = 9;
                    throw(indexNotInt);
                }
            }
            if (right.mNum > left.mArray->size() - 1 || right.mNum < 0) {
                error indexOutOfBounds;
                indexOutOfBounds.code = 8;
                throw(indexOutOfBounds);
            }
            // syntax correct, proceed to array assignment

            Value reassignVal;
            boolNum reassignBoolNum = evaluate(root->rightChild, variables);
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
                reassignVal = reassignBoolNum.mType;
            }
            shared_ptr<vector<Value>> reassignVec = left.mArray;
            (*reassignVec)[right.mNum] = reassignVal;
            // cout << "exiting evaluate" << endl;
            return reassignBoolNum;
        }
        else if (true) {
            // assignment to a funCall
        }
    }
    if (root->type == "lookUp") { // lookUp
        boolNum left = evaluate(root->leftChild, variables);
        boolNum right = evaluate(root->rightChild, variables);
        if (left.mType != "array") {
            error notArray;
            notArray.code = 6;
            throw(notArray);
        }
        if (right.mType != "num") {
            error indexNotNum;
            indexNotNum.code = 7;
            throw(indexNotNum);
        }
        if (right.mType == "num") { // check if integer
            if (fmod(right.mNum, 1) != 0) {
                error indexNotInt;
                indexNotInt.code = 9;
                throw(indexNotInt);
            }
        }
        if (right.mNum > left.mArray->size() - 1 || right.mNum < 0) {
            error indexOutOfBounds;
            indexOutOfBounds.code = 8;
            throw(indexOutOfBounds);
        }
        // syntax correct, proceed to array lookup

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
        }
        else { // else it holds a array
            result.mType = "array";
            result.mArray = get<shared_ptr<vector<Value>>>(right.mArray->at(right.mNum));
            // cout << "exiting evaluate" << endl;
        }
        return result;
    }
    else if (root->type == "op") { // operations that operate on 2 nums
        if (evaluate(root->leftChild, variables).mType != "num" || evaluate(root->rightChild, variables).mType != "num") {
            error invalidReturn;
            invalidReturn.code = 4;
            //cout << "error 15" << endl;
            throw(invalidReturn);
        }

        if (root->data == "+") {
            boolNum result("num", 0, false);
            result.mNum = evaluate(root->leftChild, variables).mNum + evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "-") {
            boolNum result("num", 0, false);
            result.mNum = evaluate(root->leftChild, variables).mNum - evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "*") {
            boolNum result("num", 0, false);
            result.mNum = evaluate(root->leftChild, variables).mNum * evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "/") {
            double right = evaluate(root->rightChild, variables).mNum;
            if (right == 0) {
                error zero;
                zero.code = 0;
                //cout << "error 16" << endl;
                throw(zero);
            }
            boolNum result( "num", 0, false);
            result.mNum = evaluate(root->leftChild, variables).mNum / evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "%") { 
            double right = evaluate(root->rightChild, variables).mNum;
            if (right == 0) {
                error zero;
                zero.code = 0;
                //cout << "error 17" << endl;
                throw(zero);
            }
            boolNum result("num", 0, false);
            result.mNum = fmod(evaluate(root->leftChild, variables).mNum, evaluate(root->rightChild, variables).mNum);
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "<") {
            boolNum result("bool", 0, false);
            if (evaluate(root->leftChild, variables).mNum < evaluate(root->rightChild, variables).mNum) {
                result.mBool = true;
            }
            else {
                result.mBool = false;
            }
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "<") {
            boolNum result("bool", 0, false);
            result.mBool = evaluate(root->leftChild, variables).mNum < evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == ">") {
            boolNum result("bool", 0, false);
            result.mBool = evaluate(root->leftChild, variables).mNum > evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "<=") {
            boolNum result("bool", 0, false);
            result.mBool = evaluate(root->leftChild, variables).mNum <= evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == ">="){
            boolNum result("bool", 0, false);
            result.mBool = evaluate(root->leftChild, variables).mNum >= evaluate(root->rightChild, variables).mNum;
            // cout << "exiting evaluate" << endl;
            return result;
        }
    }
    else if (root->type == "logicOp") { // operators that operate on two bools
        if (evaluate(root->leftChild, variables).mType != "bool" || evaluate(root->rightChild, variables).mType != "bool") {
            error invalidReturn;
            invalidReturn.code = 4;
            //cout << "error 18" << endl;
            throw(invalidReturn);
        }

        if (root->data == "&") {
            boolNum result("bool", 0, false);
            result.mBool = evaluate(root->leftChild, variables).mBool && evaluate(root->rightChild, variables).mBool;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "^") {
            boolNum result("bool", 0, false);
            result.mBool = evaluate(root->leftChild, variables).mBool ^ evaluate(root->rightChild, variables).mBool;
            // cout << "exiting evaluate" << endl;
            return result;
        }
        else if (root->data == "|") {
            boolNum result("bool", 0, false);
            result.mBool = evaluate(root->leftChild, variables).mBool || evaluate(root->rightChild, variables).mBool;
            // cout << "exiting evaluate" << endl;
            return result;
        }
    }
    else if (root->type == "eqIneq") { // operators that operate on anything
        if (evaluate(root->leftChild, variables).mType == "bool") { // left child is bool
            if (evaluate(root->rightChild, variables).mType != "bool") {
                if (root->data == "==") {
                    boolNum result("bool", 0, false);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
                if (root->data == "!=") {
                    boolNum result("bool", 0, true);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
            }

            if (root->data == "==") {
                boolNum result("bool", 0, false);
                result.mBool = evaluate(root->leftChild, variables).mBool == evaluate(root->rightChild, variables).mBool;
                // cout << "exiting evaluate" << endl;
                return result;
            }
            else if (root->data == "!="){ // else its "!="
                boolNum result("bool", 0, false);
                result.mBool = evaluate(root->leftChild, variables).mBool != evaluate(root->rightChild, variables).mBool;
                // cout << "exiting evaluate" << endl;
                return result;
            }
        }
        else if (evaluate(root->leftChild, variables).mType == "num") { // left child is a num
            if (evaluate(root->rightChild, variables).mType != "num") {
                if (root->data == "==") {
                    boolNum result("bool", 0, false);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
                if (root->data == "!=") {
                    boolNum result("bool", 0, true);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
            }
            
            if (root->data == "==") {
                boolNum result("bool", 0, false);
                result.mBool = evaluate(root->leftChild, variables).mNum == evaluate(root->rightChild, variables).mNum;
                // cout << "exiting evaluate" << endl;
                return result;
            }
            else if (root->data == "!="){
                boolNum result("bool", 0, false);
                result.mBool = evaluate(root->leftChild, variables).mNum != evaluate(root->rightChild, variables).mNum;
                // cout << "exiting evaluate" << endl;
                return result;
            }
        }
        else if (evaluate(root->leftChild, variables).mType == "null") { // left child is a null
            if (evaluate(root->rightChild, variables).mType != "null") {
                if (root->data == "==") {
                    boolNum result("bool", 0, false);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
                if (root->data == "!=") {
                    boolNum result("bool", 0, true);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
            }
            
            if (root->data == "==") {
                boolNum result("bool", 0, true);
                // cout << "exiting evaluate" << endl;
                return result;
            }
            else if (root->data == "!="){
                boolNum result("bool", 0, false);
                // cout << "exiting evaluate" << endl;
                return result;
            }
        }
        else { // left child is an array
            if (evaluate(root->rightChild, variables).mType != "array") {
                if (root->data == "==") {
                    boolNum result("bool", 0, false);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
                if (root->data == "!=") {
                    boolNum result("bool", 0, true);
                    // cout << "exiting evaluate" << endl;
                    return result;
                }
            }
            
            boolNum result("bool", 0, true);
            shared_ptr<std::vector<Value>> leftArray = evaluate(root->leftChild, variables).mArray;
            shared_ptr<std::vector<Value>> rightArray = evaluate(root->rightChild, variables).mArray;
            if (leftArray->size() != rightArray->size()) { // check if same size
                result.mBool = false;
            }
            for (unsigned i = 0; i < leftArray->size(); i++) { // check if each element is the same (uses operation overloading)
                if (leftArray->at(i) != rightArray->at(i)) {
                    result.mBool = false;
                    break;
                }
            }

            if (root->data == "!=") { // flip result if != instead of ==
                result.mBool = !result.mBool;
            }
            // cout << "exiting evaluate" << endl;
            return result;
        }
    }
    // cout << "exiting evaluate" << endl;
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