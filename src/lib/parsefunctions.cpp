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
    else { // it must be a number
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

bool inVec(vector<string> someVec, string someString) { // helper function for expressionChecker
    for (unsigned int i = 0; i < someVec.size(); i++) {
        if (someVec.at(i) == someString) {
            return 1;
        }
    }
    return 0;
}

bool isOp(string someString) { // helper function for expressionChecker
    if (someString == "+" || someString == "-" || someString == "*" || someString == "/" || someString == "=") {
        return true;
    }
    else {
        return false;
    }
}

bool isVar (string someString) {
    if (someString.at(0) == '_' || isalpha(someString.at(0))) {
        return true;
    }
    else {
        return false;
    }
}

void expressionChecker(vector<token> tokenVec){
    vector<string> definedVars;
    
    if (tokenVec.size() == 1) { // if empty (only end token)
        cout << "jim1" << endl;
        cout << "Unexpected token at line "<< tokenVec.at(0).row <<" column " << tokenVec.at(0).column << ": END" << endl;
        exit(2);
    }
    
    if (tokenVec.size() == 2) { // if its a single token + end token 
        if (isFloat(tokenVec.at(0).data)) {
            return;
        }
        else  {
            cout << "jim2" << endl;
            cout << "Unexpected token at line 1 column 1: " << tokenVec.at(0).data << endl;
            exit(2);
        }
    }

    // equations longer than one token + end token
    
    if (tokenVec.at(0).data != "(" && !isFloat(tokenVec.at(0).data)) {
        cout << "jim3" << endl;
        cout << "Unexpected token at line " << tokenVec.at(0).row << " " << "column " << tokenVec.at(0).column << ": " << tokenVec.at(0).data << endl;
        exit(2);
    }
    if (isFloat(tokenVec.at(0).data)) {
        if (isOp(tokenVec.at(1).data) || tokenVec.at(1).data == ")" || tokenVec.at(1).type == "var")) {
            cout << "jim4" << endl;
            cout << "Unexpected token at line " << tokenVec.at(1).row << " " << "column " << tokenVec.at(1).column << ": " << tokenVec.at(1).data << endl;
        }
    }

    string oldData = "";
    int parenthesisDiff = 0; // left and right parenthesis diff
    unsigned int i;

    for (i = 0; i < tokenVec.size(); i++) {
        string data = tokenVec.at(i).data;
        int row = tokenVec.at(i).row;
        int col = tokenVec.at(i).column;

        if (data == "END") {
            if (oldData != ")") {
                cout << "jim5" << endl;
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (data == ")") {
            if (oldData == "(" || isOp(oldData) || oldData == "=") {
                cout << "jim6" << endl;
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
            if (tokenVec.at(i - 1).type == "var" && !inVec(definedVars, oldData)) { // its an undefined variable
                cout << "jim7" << endl;
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (isFloat(data)) {
            if (oldData == "(" || oldData == "=") {
                cout << "jim8" << endl;
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (isOp(data)) {
            if (oldData != "(") {
                cout << "jim8" << endl;
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (data == "(") {
            if (oldData == "=" || oldData == "(") {
                cout << "jim10" << endl;
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
        }
        if (tokenVec.at(i).type == "var") { // its a variable
            if (oldData == "=") {
                definedVars.push_back(data);
            }
            if (oldData == "(") {
                cout << "jim11" << endl;
                cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                exit(2);
            }
            if (!inVec(definedVars, data)) {
                if (oldData != "=") {
                    cout << "jim12" << endl;
                    cout << "Unexpected token at line " <<  row << " column " << col << ": " << data << endl;
                    exit(2);
                }
            }
        }
        if (data == "=") {
            int newParenthesisDiff = 1; // left is +1, right is -1
            bool enteredNested = 0;
            if (tokenVec.at(i+1).type == "var") {
                cout << "jim13" << endl;
                cout << "Unexpected token at line " <<  tokenVec.at(i+1).row << " column " << tokenVec.at(i+1).column << ": " << tokenVec.at(i+1).data << endl;
                exit(2);
            }
            int j = i + 1;
            while (newParenthesisDiff != 0 || tokenVec.at(j).data != "END") {
                if (tokenVec.at(j).data == "(") {
                    enteredNested = 1;
                    newParenthesisDiff++;
                }
                else if (tokenVec.at(j).data == ")") {
                    newParenthesisDiff--;
                }
                if (enteredNested && newParenthesisDiff == 1) {
                    if (tokenVec.at(j+1).data != ")") {
                        cout << "jim14" << endl;
                        cout << "Unexpected token at line " <<  tokenVec.at(j+1).row << " column " << tokenVec.at(j+1).column << ": " << tokenVec.at(j+1).data << endl;
                        exit(2);
                    }
                }
                j++;
            }
            if ((j - i) < 3) {
                cout << "jim15" << endl;
                cout << "Unexpected token at line " <<  tokenVec.at(i+2).row << " column " << tokenVec.at(i+2).column << ": " << tokenVec.at(i+2).data << endl;
                exit(2);
            }
            if (newParenthesisDiff != 0) {
                cout << "jim16" << endl;
                cout << "Unexpected token at line " <<  tokenVec.at(j).row << " column " << tokenVec.at(j).column << ": " << tokenVec.at(j).data << endl;
                exit(2);
            }
        }

        if (data == "(") {
            parenthesisDiff++;  
        }
        if (data == ")") {
            parenthesisDiff--;
        }

        oldData = data;
    }

    // making sure there are matching left and right parenthesis
    if (parenthesisDiff != 0) {
        int column = tokenVec.at(i).column;
        int row = tokenVec.at(i).row;
        cout << "jim16" << endl;
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