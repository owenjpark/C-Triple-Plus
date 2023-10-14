#include "AST.h"

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

AST::node* parse(vector<vecComponent> lexVec, int index, string parent){
    index++; // eat the parenthesis
    int lCounter = 1;
    int rCounter = 0;
    AST::node* oper = new AST::node();
    oper->data = lexVec[index].data;
    oper->parent = parent;
    index++; // eat up operreator

    while (lCounter != rCounter) {
        if (lCounter - rCounter != 1) { // if inside statement within a statement
            if (lexVec[index].data == "(") {
                lCounter++;
            }
            else if (lexVec[index].data == ")") {
                rCounter++;
            }
            index++;
            continue;
        }
        else if (lexVec[index].data == "(") {
            lCounter++;
            oper->children.push_back(parse(lexVec, index, oper->data));
            index++;
        }
        else if (lexVec[index].data == ")") {
            rCounter++;
            index++;
        }
        else {
            AST::node* num = new AST::node();
            num->data = lexVec[index].data;
            num->parent = oper->data;

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
    else {
        cout << nodeParam->data;
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
                someValue /= childrenVal.at(i);
            }
        }
        return someValue;
    }
    else {
        return stod(nodeParam->data);
    }
}