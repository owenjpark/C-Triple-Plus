#include "parse.h"
#include "lex.h"
#include <vector>
#include <iomanip>

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

AST::node* parse(vector<vecComponent> lexVec, int index){
    if (index == 0 && lexVec.at(0).data != "(") { // edge case
        AST::node* num = new AST::node();
        num->data = lexVec.at(index).data;
        return num;
    }

    index++; // eat the parenthesis

    int lCounter = 1;
    int rCounter = 0;

    AST::node* oper = new AST::node();
    oper->data = lexVec.at(index).data;
    index++; // eat up operreator

    while (lCounter != rCounter) { // while expression incomplete
        if (lCounter - rCounter != 1) { // if inside expression within an expression
            if (lexVec.at(index).data == "(") {
                lCounter++;
            }
            else if (lexVec.at(index).data == ")") {
                rCounter++;
            }
            index++;
            continue;
        }
        else if (lexVec.at(index).data == "(") {
            lCounter++;
            oper->children.push_back(parse(lexVec, index));
            index++;
        }
        else if (lexVec.at(index).data == ")") {
            rCounter++;
            index++;
        }
        else {
            AST::node* num = new AST::node();
            num->data = lexVec.at(index).data;

            oper->children.push_back(num);
            index++;
        }
    }
    return oper;
}

void printEquation(AST::node* nodeParam) { // pass head of tree to print whole tree
    if (nodeParam->data == "+" || nodeParam->data == "-" || nodeParam->data == "*" || nodeParam->data == "/") {
        cout << "(" ;
    }
    for (unsigned i = 0; i < nodeParam->children.size(); i++) {
        printEquation(nodeParam->children.at(i));

        if (i != nodeParam->children.size() - 1) { // if last child, dont print parent
            cout << " " << nodeParam->data << " "; // print parent
        }
    }
    
    if (nodeParam->data == "+" || nodeParam->data == "-" || nodeParam->data == "*" || nodeParam->data == "/") {
        cout << ")" ;
    }
    else { // print num
        double num = stod(nodeParam->data);
        cout << num;
    }
}

double evaluate(AST::node* nodeParam) {
    double someValue = 0;
    vector<double> childrenVal;
    for (unsigned i = 0; i < nodeParam->children.size(); i++) {
        childrenVal.push_back(evaluate(nodeParam->children.at(i)));
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

void noExpression(vector<vecComponent> lexVec) {
    if (lexVec.size() == 0) {
        exit(2);
    }
}

bool isFloat(string someString) {
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

void validCheck(vector<vecComponent> lexVec){
    int countRL = 0;

    long unsigned int i; // if empty
    if (lexVec.size() == 1) {
        cout << "Unexpected token at line 1 column 1: END" << endl;
    }


    // if its a single number
    if (lexVec.size() == 2) {
        if (isFloat(lexVec[0].data)) {
            return;
        }
        else  cout << "Unexpected token at line 1 column 1: " << lexVec[0].data << endl;
    }

    
    // equations longer than one token 
    // checking if valid parenthesis and operations 
    if (lexVec[0].data != "(") {
        cout << "Unexpected token at line 1 column 1: " << lexVec[0].data << endl;
    }

    string oldData = "(";
    for (i = 1; i < lexVec.size(); i++) {
        string data = lexVec[i].data;
        int row = lexVec[i].row;
        int col = lexVec[i].column;

        // checking for valid operation
        if (oldData == "(") {
            if (data == ")" || isFloat(data)) {
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }


        if (lexVec[i].data == "(") {
            countRL += 1;  
        }

        if (lexVec[i].data == ")") {
            countRL -= 1;
        }

        if (countRL == 0 && lexVec[i+1].data != "END") // making sure there are not multiple expressions
         {
            cout << "Unexpected token at line " << lexVec[i].row << " column " << i << ": " << lexVec[i].data << endl;
            exit(2);
        }

        oldData = data;
    
    }
// making sure there are matching left and right parenthesis
    if (countRL != 0) {
        int C = lexVec[i].column;
        int L = lexVec[i].row;
        cout << "Unexpected token at line " <<  L << " column " << C << ": " << lexVec[i].data << endl; 
        exit(2);
    }

}