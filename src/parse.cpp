#include "lib/parse.h"
#include <vector>
#include <iomanip>

void lexer (const string line, const int row, vector<vecComponent> &inputVec) {
    string data;
    int column = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char lineChar = line[i];

        if (isdigit(lineChar)) {
            int dotCount = 0;
            int preservedColumn = column;
            data = "";

            while (isdigit(lineChar) || lineChar == '.') {
                if (lineChar == '.') { // if has more than one '.'
                    dotCount++;
                }
                if (dotCount > 1) { // if has more than one '.'
                    cout << "Syntax error on line " << row << " column " << column << "." << endl;
                    exit(1);
                }
                data.push_back(lineChar);
                column++;
                i++;
                lineChar = line[i];
            }

            i--;

            if (line[i] == '.') { // if ends with '.'
                cout << "Syntax error on line " << row << " column " << column << "." << endl;
                exit(1);
            }
            else { // everything is good, let's create vecComponent and push onto vector
                vecComponent num;
                num.column = preservedColumn;
                num.data = data;
                num.row = row;

                inputVec.push_back(num);
            }
        }
        else if (lineChar == '(' || lineChar == ')' || lineChar == '+' || lineChar == '-' || lineChar == '*' || lineChar == '/') {   
            vecComponent op;
            op.data = lineChar;
            op.column = column;
            op.row = row;

            column++;
            inputVec.push_back(op);
        }
        else if (lineChar == ' ' || lineChar == '\t') {
            column++;
            continue;
        }
        else {
            cout << "Syntax error on line " << row << " column " << column << "." << endl;
            exit(1);
        }
    }
}

void printer(vector<vecComponent> someVec) {
    for (unsigned int i = 0; i < someVec.size(); i++) {
        cout << right << setw(4) << someVec.at(i).row << setw(5) << someVec.at(i).column << "  " << someVec.at(i).data << endl;
    }
}

void addEnd(vector<vecComponent> &inputVec, bool wasNL) {
    int lastRow = inputVec.back().row;
    int lastCol = inputVec.back().column;

    vecComponent endComponent;

    if (wasNL) { // no newline
        endComponent.column = 1;
        endComponent.row = lastRow + 1;
        endComponent.data = "END";
    }
    else { // newline
        endComponent.column = lastCol + 1;
        endComponent.row = lastRow;
        endComponent.data = "END";
    }

    inputVec.push_back(endComponent);
}

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

int main() {
    vector<vecComponent> someVec;
    int counter = 1;
    string someLine = "(* (+ 1 2) 3 (/ 4 5 (- 6 7)))";

    lexer(someLine, counter, someVec);

    AST::node* root = parse(someVec, 0, "");

    printEquation(root);

    cout << endl;

    cout << evaluate(root);

    return 0;
}
