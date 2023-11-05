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
    if (code ==1) exit(1);

    // have to add errors 
    
    unique_ptr<AST3::Node> programRoot; 
    try { // build tree
        programRoot = buildProgram(tokenInput);
    }
    catch (error Error){
        if (Error.code == 2) {
            cout << "Unexpected token at line " << Error.row << " column " << Error.column << ": " << Error.data << endl;
            code = 2;
        }
    };
    if (code == 2) exit(2);

    // cout << "done buildingProgram" << endl;
    // for (unsigned i = 0; i < programRoot->children.size(); i++) {
    //     cout << "programRoot child #" << i + 1 << ": " << programRoot->children.at(i)->data << endl;
    //     for (unsigned j = 0; j < programRoot->children.at(i)->children.size(); j++) {
    //         cout << programRoot->children.at(i)->data << ": " << programRoot->children.at(i)->children.at(j)->data << endl;
    //     }
    //     cout << endl;
    // }

    try {
        runProgram(programRoot, variables);
    }
    catch (error runtime) {
        if(runtime.code == 3) code =3;
    }
    if (code == 3) exit(3);

    return 0;
}