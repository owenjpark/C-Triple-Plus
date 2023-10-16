#include "lib/lex.h"

int main() {
    vector<token> tokenVec;

    tokenVec = lexer();

    printTokens(tokenVec);

    return 0;
}