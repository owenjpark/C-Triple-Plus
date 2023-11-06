
#include "lib/calc.h"
#include "lib/lex.h"
#include "lib/scrypt.h"
#include "lib/format.h"


int main(){
    
    vector<token> tokenInput;
    vector<variable> variables;
    int code = 0;
    //getting token version of the input 
    try{
        tokenInput = lexer();
    }
    catch(error Error){
        if (Error.code == 1) {
            cout << "Syntax error on line " << Error.row << " column " << Error.column << "." << endl;
            code = 1;
        }
    }
    if (code ==1) exit(1);
    //for (int i = 0; i< int(tokenInput.size()); i++) {
      //  cout << tokenInput[i].data << " at " << tokenInput[i].row << endl;
    //}
    // have to add errors 
    
    unique_ptr<AST3::Node> programRoot; 
    try { // build tree
        programRoot = buildProgram(tokenInput);
    }
    catch (error Error){
        if (Error.code == 2) {
            cout << "Unexpected token at line " << Error.row <<" column " << Error.column << ": " << Error.data << endl;
            code = 2;
        }
    };
    if (code == 2) exit(2);
    //cout << "not seg faulting in build" << endl;
    //cout << "third child: " << programRoot->children[0]->children[2]->children[0]->data << endl;
    try {
        runProgram(programRoot, variables);
    }
    catch (error runtime) {
        if(runtime.code == 3) code =3;
    }
    if (code ==3) exit(3);

    //for (int j = 0; j < int(variables.size()); j++) {
    //    cout << "variable: " << variables[j].name << endl;
    //} 

    //printStatements(tokenInput);

    //cout << "1: " << programRoot->children[0]->data << "and" << programRoot->children[1]->data <<endl;
    //cout << "in main: " << programRoot->children[0]->data << endl;
    //cout << "hello" << endl;
    //start evaluating 
    // 

// if expression build and evaluate 
// if while make sure 
// if if/while doesn't evaluate to bool runtime error "Runtime error: condition is not a bool."
// exit code 3 
// lexer error --lexer handles it exits with code 1
// parse error exit code 2



    return 0;
}