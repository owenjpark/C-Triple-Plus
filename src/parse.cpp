#include "lib/parse.h"
#include "lib/lex.h"



int main() {
    vector<token> tokenVec;

    try {
        tokenVec = lexer();
    }
    catch (error someError){
        cout << "Syntax error on line " << someError.row << " column " << someError.column << "." << endl;
        exit(someError.code);
    }
    try {
        parser(tokenVec);
    }
    catch (error someError) {
        if (someError.code == 2) {
            cout << "Unexpected token at line " << someError.row << " column " << someError.column << ": " << someError.data << endl;
            exit(someError.code);
        }
        else { // else exit(3)
            exit(someError.code);
        }
        
    }
    

    return 0;
}