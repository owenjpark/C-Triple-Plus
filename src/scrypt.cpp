#include "lib/calc.h"
#include "lib/lex.h"
#include "lib/scrypt.h"
#include "lib/format.h"


int main() {
    
    vector<token> tokenInput;
    vector<variable> variables;
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
    
    // building tree
    unique_ptr<AST3::Node> programRoot; 
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

    // running program
    try {
        runProgram(programRoot, variables);
    }
    catch (error runtime) {
        if (runtime.code == 0) { //
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
        exitCode = 3;
    }
    if (exitCode == 3) {
        exit(3);
    }

    return 0;
}