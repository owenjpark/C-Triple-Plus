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

AST::node* createAST(vector<token> tokenVec, int vectorIndex){ // assumes valid expression
    if (vectorIndex == 0 && tokenVec.at(0).data != "(") { // edge case: input is only a #, no operations performed e.g. "12"
        AST::node* num = new AST::node();
        num->data = tokenVec.at(vectorIndex).data;
        return num;
    }

    vectorIndex++;
    // at index of operator

    AST::node* oper = new AST::node();
    oper->data = tokenVec.at(vectorIndex).data;
    vectorIndex++;
    // at index of first operand

    int lParenthesisCount = 1;
    int rParenthesisCount = 0;

    while (lParenthesisCount != rParenthesisCount) { 
        if (lParenthesisCount - rParenthesisCount != 1) { // if inside a nested expression set index to go past it (recursion deals with it so we can ignore it)
            if (tokenVec.at(vectorIndex).data == "(") {
                lParenthesisCount++;
            }
            else if (tokenVec.at(vectorIndex).data == ")") {
                rParenthesisCount++;
            }
            vectorIndex++;
            continue;
        }
        else if (tokenVec.at(vectorIndex).data == "(") { // we have reached a nested expression, perform recursion
            lParenthesisCount++;
            oper->children.push_back(createAST(tokenVec, vectorIndex));
            vectorIndex++;
        }
        else if (tokenVec.at(vectorIndex).data == ")") {
            rParenthesisCount++;
            vectorIndex++;
        }
        else { // if its a double, push directly onto children vector of operator
            AST::node* num = new AST::node();
            num->data = tokenVec.at(vectorIndex).data;

            oper->children.push_back(num);
            vectorIndex++;
        }
    }
    return oper;
}

void printInfix(AST::node* someNode) {
    if (someNode->data == "+" || someNode->data == "-" || someNode->data == "*" || someNode->data == "/") {
        cout << "(" ;
    }
    for (unsigned i = 0; i < someNode->children.size(); i++) {
        printInfix(someNode->children.at(i));

        if (i != someNode->children.size() - 1) {
            cout << " " << someNode->data << " "; // dont print operator if last child 
        }
    }
    
    if (someNode->data == "+" || someNode->data == "-" || someNode->data == "*" || someNode->data == "/") {
        cout << ")" ;
    }
    else { // else it is a number
        double num = stod(someNode->data);
        cout << num;
    }
}

double evaluateAST(AST::node* someNode) {
    double totalChildrenVal = 0;
    vector<double> childrenVals;

    for (unsigned i = 0; i < someNode->children.size(); i++) {
        childrenVals.push_back(evaluateAST(someNode->children.at(i)));
    }

    if (someNode->data == "+") {
        for (unsigned i = 0 ; i < childrenVals.size(); i++) {
            if (i == 0) { // if first operand
                totalChildrenVal = childrenVals.at(i);
            }
            else {
                totalChildrenVal += childrenVals.at(i);
            }
        }
        return totalChildrenVal;
    }
    else if (someNode->data == "-"){
        for (unsigned i = 0 ; i < childrenVals.size(); i++) {
            if (i == 0) { // if first operand
                totalChildrenVal = childrenVals.at(i);
            }
            else {
                totalChildrenVal -= childrenVals.at(i);
            }
        }
        return totalChildrenVal;
    }
    else if (someNode->data == "*"){
        for (unsigned i = 0 ; i < childrenVals.size(); i++) {
            if (i == 0) { // if first operand
                totalChildrenVal = childrenVals.at(i);
            }
            else {
                totalChildrenVal *= childrenVals.at(i);
            }
        }
        return totalChildrenVal;
    }
    else if (someNode->data == "/"){
        for (unsigned i = 0 ; i < childrenVals.size(); i++) {
            if (i == 0) { // if first operand
                totalChildrenVal = childrenVals.at(i);
            }
            else {
                if (childrenVals.at(i) == 0) {
                    cout << "\nRuntime error: division by zero." << endl;
                    exit(3);
                }
                totalChildrenVal /= childrenVals.at(i);
            }
        }
        return totalChildrenVal;
    }
    else {
        return stod(someNode->data);
    }
}

bool isFloat(string someString) { // helper function for expressionChecker
    for (unsigned i = 0; i < someString.size(); i++) {
        if (isdigit(someString.at(i)) || someString.at(i) == '.') {
            continue;
        }
        else {
            return false;
        }  
    }
    return true;
}

bool isOp(string someString) { // helper function for expressionChecker
    if (someString == "+" || someString == "-" || someString == "*" || someString == "/") {
        return true;
    }
    else {
        return false;
    }
}

void expressionChecker(vector<token> tokenVec){
    // if empty
    if (tokenVec.size() == 1) {
        cout << "Unexpected token at line "<< tokenVec.at(0).row <<" column " << tokenVec.at(0).column << ": END" << endl;
        exit(2);
    }

    // if its a single number
    if (tokenVec.size() == 2) {
        if (isFloat(tokenVec.at(0).data)) {
            return;
        }
        else  {
            cout << "Unexpected token at line 1 column 1: " << tokenVec.at(0).data << endl;
            exit(2);
            }
    }

    // equations longer than one token 
    if (isFloat(tokenVec.at(0).data)) { // edge case: checking first token is float
        cout << "Unexpected token at line " << tokenVec.at(1).row << " " << "column " << tokenVec.at(1).column << ": " << tokenVec.at(1).data << endl;
        exit(2);
    }
    if (tokenVec.at(0).data != "(") {
        cout << "Unexpected token at line " << tokenVec.at(0).row << " " << "column " << tokenVec.at(0).column << ": " << tokenVec.at(0).data << endl;
        exit(2);
    }

    string oldData = "";
    int parenthesisDiff = 0;
    bool expressionDone = false;
    unsigned int i;

    for (i = 0; i < tokenVec.size(); i++) {
        string data = tokenVec.at(i).data;
        int row = tokenVec.at(i).row;
        int col = tokenVec.at(i).column;

        if (expressionDone == true) {
            if (data != "END") {
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (data == "END") {
            if (oldData != ")") {
                cout << "Unexpected token at line " <<  row << " column " << col + 1 << ": " << data << endl;
                exit(2);
            }
        }
        if (data == ")") { 
            if (oldData == "(" || isOp(oldData)) {
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (isFloat(data)) {
            if (oldData == "(" || oldData == "(") {
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (isOp(data)) {
            if (oldData == ")" || isOp(oldData) || isFloat(oldData)) {
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }

        // for multiple expressions check
        if (data == "(") {
            parenthesisDiff++;  
        }
        if (data == ")") {
            parenthesisDiff--;
        }
        if (parenthesisDiff == 0) {
            expressionDone = true;
        }

        oldData = data;
    }
    
    // making sure there are matching left and right parenthesis
    if (parenthesisDiff != 0) {
        int column = tokenVec.at(i).column;
        int row = tokenVec.at(i).row;
        cout << "Unexpected token at line " << row << " column " << column << ": " << tokenVec.at(i).data << endl; 
        exit(2);
    }
}

AST parser (vector<token> tokenVec) {
    expressionChecker(tokenVec);

    AST someAST;
    someAST.root = createAST(tokenVec, 0);

    return someAST;
}