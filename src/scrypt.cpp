#include "lib/calc.h"
#include "lib/lex.h"
#include "lib/scrypt.h"
#include "lib/format.h"


int main(){
    
    vector<token> tokenInput;
    vector<variable> variables;
    int code = 0;
    // getting token version of the input 
    try{
        tokenInput = lexer();
    }
    catch(error Error){
        if (Error.code == 1) {
            cout << "Syntax error on line " << Error.row << " column " << Error.column << "." << endl;
            code = 1;
        }
    }
    if (code ==1) {
        exit(1);
    }
    
    unique_ptr<AST3::Node> programRoot; 
    try { // build tree
        programRoot = buildProgram(tokenInput);
    }
    catch (error Error){
        if (Error.code == 2) {
            cout << "Unexpected token at line " << Error.row << " column " << Error.column << ": " << Error.data << endl;
            code = 2;
        }
    }
    if (code == 2) { 
        exit(2);
    }

    // cout << "here: " << programRoot->children.at(3)->children.at(4)->data << endl;

    try {
        runProgram(programRoot, variables);
    }
    catch (error runtime) {
        if(runtime.code == 3) code =3;
    }
    if (code == 3) {
        exit(3);
    }

    return 0;
}