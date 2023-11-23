#include "lib/calc.h"
#include "lib/lex.h"
#include "lib/scrypt.h"
#include "lib/format.h"


int main() {
    vector<token> tokenInput;
    vector<variable> variables;

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

    int exitCode = 0;

    // getting token version of the input 
    try{
        tokenInput = lexer();
    }
    catch(error syntaxError) {
        if (syntaxError.code == 1) {
            cout << "Syntax error on line " << syntaxError.row << " column " << syntaxError.column << "." << endl;
            exitCode = 1;
        }
    }
    if (exitCode ==1) {
        exit(1);
    }

    // // building tree
    shared_ptr<AST3::Node> programRoot; 
    try { 
        programRoot = buildProgram(tokenInput);
    }
    catch (error parseError){
        if (parseError.code == 2) {
            cout << "Unexpected token at line " << parseError.row << " column " << parseError.column << ": " << parseError.data << endl;
            exitCode = 2;
        }
    }
    if (exitCode == 2) { 
        exit(2);
    }

    try {
        validReturn(programRoot, false);
    }
    catch (error runtime) {
        if (runtime.code == 11) {
            cout << "Runtime error: unexpected return." << endl;
        }
        exitCode = 3;
    }
    if (exitCode == 3) {
        exit(3);
    }

    try {
        runProgram(programRoot, variables);
    }
    catch (error runtime) {
        if (runtime.code == 0) {
            cout << "Runtime error: division by zero."  << endl;
        }
        if (runtime.code == 3) {
            cout << "Runtime error: unknown identifier " << runtime.data << endl;
        }
        if (runtime.code == 4) {
            cout << "Runtime error: invalid operand type." << endl;
        }
        if (runtime.code == 5) {
            cout << "Runtime error: condition is not a bool." << endl;
        }
        if (runtime.code == 10) {
            cout << "Runtime error: incorrect argument count." << endl;
        }
        if (runtime.code == 12) {
            cout << "Runtime error: not a function." << endl;
        }
        exitCode = 3;
    }
    if (exitCode == 3) {
        exit(3);
    }

    return 0;
}