#include "lib/calc.h"
#include "lib/lex.h"

int main() {
    string line;
    vector<variable> variables;

    // initializing variables for specialized functions: pop(), len(), push()
    variable popParam;
    popParam.name = "x";
    popParam.type = "parameter";
    variable pop;
    pop.name = "pop";
    pop.type = "special";
    pop.funcVal.localScope.push_back(popParam);
    variables.push_back(pop);

    variable lenParam;
    lenParam.name = "y";
    lenParam.type = "parameter";
    variable len;
    len.name = "len";
    len.type = "special";
    len.funcVal.localScope.push_back(lenParam);
    variables.push_back(len);

    variable pushParam1;
    variable pushParam2;
    pushParam1.name = "z";
    pushParam2.name = "q";
    pushParam1.type = "parameter";
    pushParam2.type = "parameter";
    variable push;
    push.name = "push";
    push.type = "special";
    push.funcVal.localScope.push_back(pushParam1);
    push.funcVal.localScope.push_back(pushParam2);
    variables.push_back(push);


    while(std::getline(std::cin, line)) {
        vector<token> tokenVec;
        try { // lex
            createTokens(line, 1, tokenVec);
        }
        catch (error Error){
            if (Error.code == 1) {
                cout << "Syntax error on line " << Error.row << " column " << Error.column << "." << endl;
            }
            continue;
        }
        addEndToken(tokenVec, 1, line.size() + 1);

        try {
            expressionChecker(0, tokenVec.size() - 1, tokenVec);
        }
        catch (error Error){
            if (Error.code == 2) {
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
            }
            continue;
        }
        
        AST2 tree;
        try { // build tree
            tree.root = build(tokenVec);
        }
        catch (error Error){
            if (Error.code == 2) {
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
            }
            continue;
        }
                
        printInfix(tree.root);
        cout << endl;

        vector<variable> temp = variables; // copy of variables vector in case of "no update on error"
        try { // evaluate answer
            boolNum result = evaluate(tree.root, temp);
            if (result.mType == "num") {
                cout << result.mNum << endl;
            }
            else if (result.mType == "bool") { // else if its a bool
                if (result.mBool == true) {
                    cout << "true" << endl;
                }
                else {
                    cout << "false" << endl;
                }
            }
            else if (result.mType == "null") { // else if its a null
                cout << "null" << endl;
            }
            else { // else its an array
                arrayPrinter(result.mArray);
                cout << endl;
            }
        }
        catch (error runtime){
            if (runtime.code == 0) {
                cout << "Runtime error: division by zero."  << endl;
            }
            else if (runtime.code == 3) {
                cout << "Runtime error: unknown identifier " << runtime.data << endl;
            }
            else if (runtime.code == 4) {
                cout << "Runtime error: invalid operand type." << endl;
            }
            else if (runtime.code == 5) {
                cout << "Runtime error: invalid assignee." << endl;
            }
            else if (runtime.code == 6) {
                cout << "Runtime error: not an array." << endl;
            }
            else if (runtime.code == 7) {
                cout << "Runtime error: index is not a number." << endl;
            }
            else if (runtime.code == 8) {
                cout << "Runtime error: index out of bounds." << endl;
            }
            else if (runtime.code == 9) {
                cout << "Runtime error: index is not an integer." << endl;
            }
            else if (runtime.code == 10) {
                cout << "Runtime error: incorrect argument count." << endl;
            }
            else if (runtime.code == 13) {
                cout << "Runtime error: underflow." << endl;
            }
            continue;
        }
        variables = temp; // no runtime errors, set variables to temp
    }
   
    return 0;
}