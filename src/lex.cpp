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

    printTokens(tokenVec);

    return 0;
}