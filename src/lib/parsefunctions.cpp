#include "parse.h"
#include "lex.h"

AST::AST(){
    root = nullptr;
}

void destructorHelper(AST::node* someNodeeter) {
    for (unsigned int i = 0; i < someNodeeter->children.size(); i++) {
        destructorHelper(someNodeeter->children.at(i));
    }
    delete someNodeeter;
}

AST::~AST(){
    destructorHelper(root);
}

AST::node* createAST(vector<token> tokenVec, int index){
    if (index == 0 && tokenVec.at(0).data != "(") { // edge case: if one token thats a single number e.g. "12"
        AST::node* num = new AST::node();
        num->data = tokenVec.at(index).data;
        num->type = tokenVec.at(index).type;
        return num;
    }

    index++;
    // index at operator

    int lParenthesisCount = 1;
    int rParenthesisCounter = 0;

    AST::node* oper = new AST::node();
    oper->data = tokenVec.at(index).data;
    oper->type = tokenVec.at(index).type;
    index++;
    // index at first operand

    while (lParenthesisCount != rParenthesisCounter) {
        if (lParenthesisCount - rParenthesisCounter != 1) { // if in nested operation
            if (tokenVec.at(index).data == "(") {
                lParenthesisCount++;
            }
            else if (tokenVec.at(index).data == ")") {
                rParenthesisCounter++;
            }
            index++;
            continue;
        }
        else if (tokenVec.at(index).data == "(") {
            lParenthesisCount++;
            oper->children.push_back(createAST(tokenVec, index));
            index++;
        }
        else if (tokenVec.at(index).data == ")") { // might be redundant?
            rParenthesisCounter++;
            index++;
        }
        else {
            AST::node* num = new AST::node();
            num->type = tokenVec.at(index).type;
            num->data = tokenVec.at(index).data;

            oper->children.push_back(num);
            index++;
        }
    }
    return oper;
}

void printInfix(AST::node* someNode) {
    if (someNode->type == "op" || someNode->type == "eq") {
        cout << "(" ;
    }

    for (unsigned i = 0; i < someNode->children.size(); i++) {
        printInfix(someNode->children.at(i));

        if (i != someNode->children.size() - 1) {
            cout << " " << someNode->data << " "; // as long as not last child, print parent operator between operands
        }
    }

    if (someNode->type == "op" || someNode->type == "eq") {
        cout << ")" ;
    }
    else if (someNode->type == "var") {
        cout << someNode->data;
    }
    else { // else if its a number
        double num = stod(someNode->data);
        cout << num;
    }
}

double evaluateAST(AST::node* someNode, vector<definedVar> &definedVars) {
    vector<int> erasedIndexes;

    if (someNode->data == "=") {
        for (unsigned i = 0; i < someNode->children.size() - 1; i++) { // iterate through everything except last element (expression)
            definedVar someVar;
            
            // cout << "defined: " << someNode->children.at(i)->data << endl;
            someVar.ID = someNode->children.at(i)->data;
            someVar.value = evaluateAST(someNode->children.back(), definedVars);
            definedVars.push_back(someVar);
        }
        return evaluateAST(someNode->children.back(), definedVars);
    }

    double someValue = 0;
    vector<double> childrenVal;
    for (unsigned i = 0; i < someNode->children.size(); i++) {
        childrenVal.push_back(evaluateAST(someNode->children.at(i), definedVars));
    }
    if (someNode->data == "+") {
        for (unsigned i = 0 ; i < childrenVal.size(); i++) {
            if (i == 0) { // first value
                someValue = childrenVal.at(i);
            }
            else {
                someValue += childrenVal.at(i);
            }
        }
        return someValue;
    }
    else if (someNode->data == "-"){
        for (unsigned i = 0 ; i < childrenVal.size(); i++) {
            if (i == 0) { // first value
                someValue = childrenVal.at(i);
            }
            else {
                someValue -= childrenVal.at(i);
            }
        }
        return someValue;
    }
    else if (someNode->data == "*"){
        for (unsigned i = 0 ; i < childrenVal.size(); i++) {
            if (i == 0) { // first value
                someValue = childrenVal.at(i);
            }
            else {
                someValue *= childrenVal.at(i);
            }
        }
        return someValue;
    }
    else if (someNode->data == "/"){
        for (unsigned i = 0 ; i < childrenVal.size(); i++) {
            if (i == 0) { // first value
                someValue = childrenVal.at(i);
            }
            else {
                if (childrenVal.at(i) == 0) {
                    cout << "Runtime error: division by zero." << endl;
                    exit(3);
                }
                someValue /= childrenVal.at(i);
            }
        }
        return someValue;
    }
    else if (someNode->type == "var") {
        for (int i = definedVars.size() - 1; i >= 0; i--) { // iterate through backwards to find most recent
            if (definedVars.at(i).ID == someNode->data) {
                return definedVars.at(i).value;
            }
        }
        cout << "Runtime error: unknown identifier " << someNode->data << endl;
        exit(3);
        return 0; // to avoid warning
    }
    else { // it must be a number
        return stod(someNode->data);
    }
}

void expressionChecker(int i, vector<token> &tokenVec) { // checks 1 expression at a time
    if (tokenVec.at(i).type == "num" || tokenVec.at(i).type == "var") { // expression is just a number or variable e.g. "2" or "x"
        return;
    }
    // check if expression starts with "("
    if (tokenVec.at(i).type != "lParenth") {
        cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
        exit(2);
    }

    i++;
    // at operator index
    if (tokenVec.at(i).type != "op" && tokenVec.at(i).type != "eq") {
        cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
        exit(2);
    }
    int parenthDiff = 1; // left parenthesis count - right parenthesis count

    // split into 2 cases, operator is "+ - * /" or "="
    if (tokenVec.at(i).type == "op") {
        i++;
        // at first operand
        int paramCounter = 0; // used to check that operator has at least 1 parameters
        while (parenthDiff != 0 && tokenVec.at(i).type != "end") {
            if (parenthDiff > 1) { // we are in nested expression, skip over it e.g. (+2 3) is nested expression in (* 1(+2 3)) 
                if (tokenVec.at(i).data == "(") {
                    parenthDiff++;
                }
                else if (tokenVec.at(i).data == ")") {
                    parenthDiff--;
                }
                i++;
                continue;
            }
            else if (tokenVec.at(i).type == "lParenth") { // deals with nested expression
                parenthDiff++;
                expressionChecker(i, tokenVec);
                paramCounter++;
            }
            else if (tokenVec.at(i).type == "rParenth") {
                parenthDiff--;
            }
            else if (tokenVec.at(i).type == "num" || tokenVec.at(i).type == "var") {
                paramCounter++;
            }
            else { // must be an operator or eq
                cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
                exit(2);
            }
            i++;
        }
        if (paramCounter < 1) {
            cout << "Unexpected token at line " << tokenVec.at(i - 1).row << " column " << tokenVec.at(i - 1).column << ": " << tokenVec.at(i - 1).data << endl;
            exit(2);
        }
        if (parenthDiff != 0) {
            cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
            exit(2);
        }
    }
    else { // its "="
        i++;
        // at first operand
        if (tokenVec.at(i).type != "var") {
            cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl; 
            exit(2);   
        }
        i++;
        // at 2nd operand
        bool lastParam = false; // makes a mark of when we reached last parameter e.g. 12 is last parameter in (= x y 12)
        int paramCounter = 1;
        while (parenthDiff != 0 && tokenVec.at(i).type != "end") {
            if (parenthDiff > 1) { // we are in nested expression, skip over it 
                if (tokenVec.at(i).data == "(") {
                    parenthDiff++;
                }
                else if (tokenVec.at(i).data == ")") {
                    parenthDiff--;
                }
                i++;
                continue;
            }
            else if (lastParam == 1 && tokenVec.at(i).type != "rParenth") {
                cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
                exit(2);
            }
            else if (tokenVec.at(i).type == "lParenth") {
                parenthDiff++;
                expressionChecker(i, tokenVec);
                lastParam = 1;
                paramCounter++;
            }
            else if (tokenVec.at(i).type == "rParenth") {
                parenthDiff--;
            }
            else if (tokenVec.at(i).type == "num") {
                lastParam = 1;
                paramCounter++;
            }
            else if (tokenVec.at(i).type == "var") {
                paramCounter++;
            } 
            i++;
        }
        if (paramCounter < 2) {
            cout << "Unexpected token at line " << tokenVec.at(i - 1).row << " column " << tokenVec.at(i - 1).column << ": " << tokenVec.at(i - 1).data << endl;
            exit(2);
        }
        if (parenthDiff != 0) {
            cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
            exit(2);
        }
    }
}

void parser(vector<token> tokenVec) {
    vector<definedVar> definedVars;

    if (tokenVec.size() == 1) { // no tokens
        cout << "Unexpected token at line " << tokenVec.at(0).row << " column " << tokenVec.at(0).column << ": " << tokenVec.at(0).data << endl;
    }

    int parenthDiff = 0;
    for (unsigned i = 0; i < tokenVec.size() - 1; i++) { // checking if valid trees can be constructed; iterator stops at -1 because of end token
        if (parenthDiff == 0) {
            expressionChecker(i, tokenVec);
        }
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
    }

    for (unsigned i = 0; i < tokenVec.size() - 1; i++) { // contructing trees, printing infix, and printing answers; iterator stops at -1 because of end token
        if (parenthDiff == 0) {
            AST tree;
            tree.root = createAST(tokenVec, i);
            printInfix(tree.root);
            cout << endl;
            cout << evaluateAST(tree.root, definedVars) << endl;
        }
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
    }
}