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
        else if (tokenVec.at(index).data == ")") { // might be redundant?
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
                    // exit(3);
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

void expressionChecker(int i, vector<token> &tokenVec, vector<string> &definedVars) { // does it by 1 expression at a time
    // expression can either be (), single num, or defined var
    if (tokenVec.at(i).type == "num") {
        // its  num/defined var, so next it has to be (, num, or var next
        return;
    }
    if (tokenVec.at(i).type == "var") {
        if (inVec(definedVars, tokenVec.at(i).data)) {
            return;
        }
        else {
            cout << "Runtime error: unknown identifier ID" << endl;
            exit(3);
        }
    }

    if (tokenVec.at(i).type != "lParenth") {
        cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
        exit(2);
    }

    // it has to be "("
    i++;
    // should be at operator - should split here into op and =
    if (tokenVec.at(i).type != "op" && tokenVec.at(i).type != "eq") {
        cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
        exit(2);
    }
    int parenthDiff = 1;

    if (tokenVec.at(i).type == "op") {
        i++;
        // at first operand
        if (tokenVec.at(i).type == "rParenth") { // if operator doesn't have at least 1 param REDUNDANT?
            cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
            exit(2);
        }
        int opParamCounter = 1;
        while (parenthDiff != 0 && tokenVec.at(i).type != "end") {
            if (parenthDiff > 1) { // we are IN nested expression skip over it 
                if (tokenVec.at(i).data == "(") {
                    parenthDiff++;
                }
                else if (tokenVec.at(i).data == ")") {
                    parenthDiff--;
                }
                i++;
                continue;
            }
            else if (tokenVec.at(i).type == "lParenth") { 
                parenthDiff++;
                expressionChecker(i, tokenVec, definedVars);
                opParamCounter++;
            }
            else if (tokenVec.at(i).type == "rParenth") {
                parenthDiff--;
            }
            else if (tokenVec.at(i).type == "num" || inVec(definedVars, tokenVec.at(i).data)) {
                opParamCounter++;
            }
            else if (tokenVec.at(i).type == "var") {
                cout << "Runtime error: unknown identifier ID" << endl;
                exit(3);
            }
            else if (!inVec(definedVars, tokenVec.at(i).data)) { // must be an operator or eq
                cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
                exit(2);
            }
            i++;
        }
        if (opParamCounter < 2) {
            cout << "Unexpected token at line " << tokenVec.at(i - 1).row << " column " << tokenVec.at(i - 1).column << ": " << tokenVec.at(i - 1).data << endl;
            exit(2);
        }
        if (parenthDiff != 0) {
            cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
            exit(2);
        }
    }
    else { // its eq
        vector <string> localDefinedVars;
        i++;
        // at first operand
        if (tokenVec.at(i).type != "var") {
            cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl; 
            exit(2);   
        }
        localDefinedVars.push_back(tokenVec.at(i).data);
        i++;
        // at 2nd operand
        bool lastParam = 0;
        int eqParamCounter = 1;
        while (parenthDiff != 0 && tokenVec.at(i).type != "end") {
            if (parenthDiff > 1) { // we are IN nested expression skip over it 
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
                expressionChecker(i, tokenVec, definedVars);
                lastParam = 1;
                eqParamCounter++;
            }
            else if (tokenVec.at(i).type == "rParenth") {
                parenthDiff--;
            }
            else if (tokenVec.at(i).type == "num") {
                lastParam = 1;
                eqParamCounter++;
            }
            else if (tokenVec.at(i).type == "var") {
                if (tokenVec.at(i + 1).type == "rParenth" && !inVec(definedVars, tokenVec.at(i).data)) { // TODO: throw runtime error
                    cout << "Unexpected token at line " << tokenVec.at(i + 1).row << " column " << tokenVec.at(i + 1).column << ": " << tokenVec.at(i +1).data << endl;
                    exit(2);
                }
                else {
                    eqParamCounter++;
                    localDefinedVars.push_back(tokenVec.at(i).data);
                }
            } 
            i++;
        }
        for (unsigned i = 0; i < localDefinedVars.size(); i++) {
            definedVars.push_back(localDefinedVars.at(i));
        }
        if (eqParamCounter < 2) {
            cout << "Unexpected token at line " << tokenVec.at(i - 1).row << " column " << tokenVec.at(i - 1).column << ": " << tokenVec.at(i - 1).data << endl;
            exit(2);
        }
        if (parenthDiff != 0) {
            cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " << tokenVec.at(i).data << endl;
            exit(2);
        }
    }
}

AST parser (int i, vector<token> tokenVec, vector<string> definedVars) {
    expressionChecker(i, tokenVec, definedVars);

    AST someAST;
    someAST.root = createAST(tokenVec, 0);

    return someAST;
}