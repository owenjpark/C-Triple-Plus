#include "parse.h"
#include "lex.h"

AST::AST(){
    root = nullptr;
}

void destructorHelper(AST::node* nodeParameter) {
    for (unsigned int i = 0; i < nodeParameter->children.size(); i++) {
        destructorHelper(nodeParameter->children.at(i));
    }
    delete nodeParameter;
}

AST::~AST(){
    destructorHelper(root);
}

AST::node* createAST(vector<token> tokenVec, int index){
    if (index == 0 && tokenVec.at(0).data != "(") { // edge case: if one token thats a single number e.g. "12"
        AST::node* num = new AST::node();
        num->data = tokenVec.at(index).data;
        return num;
    }

    index++;
    // index at operator

    int lParenthesisCount = 1;
    int rParenthesisCounter = 0;

    AST::node* oper = new AST::node();
    oper->data = tokenVec.at(index).data;
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
        else if (tokenVec.at(index).data == ")") {
            rParenthesisCounter++;
            index++;
        }
        else {
            AST::node* num = new AST::node();
            num->data = tokenVec.at(index).data;

            oper->children.push_back(num);
            index++;
        }
    }
    return oper;
}

void printInfix(AST::node* nodeParam) {
    if (nodeParam->data == "+" || nodeParam->data == "-" || nodeParam->data == "*" || nodeParam->data == "/") {
        cout << "(" ;
    }
    for (unsigned i = 0; i < nodeParam->children.size(); i++) {
        printInfix(nodeParam->children.at(i));

        if (i != nodeParam->children.size() - 1) {
            cout << " " << nodeParam->data << " "; // as long as not last child, print parent operator between operands
        }
    }
    
    if (nodeParam->data == "+" || nodeParam->data == "-" || nodeParam->data == "*" || nodeParam->data == "/") {
        cout << ")" ;
    }
    else { // else if its a number
        double num = stod(nodeParam->data);
        cout << num;
    }
}

double evaluateAST(AST::node* nodeParam) {
    double someValue = 0;
    vector<double> childrenVal;
    for (unsigned i = 0; i < nodeParam->children.size(); i++) {
        childrenVal.push_back(evaluateAST(nodeParam->children.at(i)));
    }
    if (nodeParam->data == "+") {
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
    else if (nodeParam->data == "-"){
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
    else if (nodeParam->data == "*"){
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
    else if (nodeParam->data == "/"){
        for (unsigned i = 0 ; i < childrenVal.size(); i++) {
            if (i == 0) { // first value
                someValue = childrenVal.at(i);
            }
            else {
                if (childrenVal.at(i) == 0) {
                    cout << "\nRuntime error: division by zero." << endl;
                    exit(3);
                }
                someValue /= childrenVal.at(i);
            }
        }
        return someValue;
    }
    else {
        return stod(nodeParam->data);
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
    if (tokenVec.size() == 1) { // if empty
        cout << "Unexpected token at line "<< tokenVec.at(0).row <<" column " << tokenVec.at(0).column << ": END" << endl;
        exit(2);
    }
    
    if (tokenVec.size() == 2) { // if its a single number
        if (isFloat(tokenVec.at(0).data)) {
            return;
        }
        else  {
            cout << "Unexpected token at line 1 column 1: " << tokenVec.at(0).data << endl;
            exit(2);
            }
    }

    // equations longer than one token 
    if (isFloat(tokenVec.at(0).data)) {
        cout << "Unexpected token at line " << tokenVec.at(1).row << " " << "column " << tokenVec.at(1).column << ": " << tokenVec.at(1).data << endl;
        exit(2);
    }
    
    if (tokenVec.at(0).data != "(") {
        cout << "Unexpected token at line " << tokenVec.at(0).row << " " << "column " << tokenVec.at(0).column << ": " << tokenVec.at(0).data << endl;
        exit(2);
    }

    string oldData = "";
    int parenthesisDiff = 0; // left and right parenthesis diff
    unsigned int i;
    bool last = 0;

    for (i = 0; i < tokenVec.size(); i++) {
        string data = tokenVec.at(i).data;
        int row = tokenVec.at(i).row;
        int col = tokenVec.at(i).column;

        if (last) {
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
            last = 1;
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