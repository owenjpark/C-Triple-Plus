#include "lib/lex.h"

int main() {
    vector<token> tokenVec;
    
    try {
        tokenVec = lexer();
    }
    catch (int exitCode){
        exit(exitCode);
    }

    printTokens(tokenVec);

    return 0;
}