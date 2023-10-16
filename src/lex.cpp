#include "lib/lex.h"
#include <vector>
#include <iomanip>

int main() {
    vector<token> tokenVec;

    tokenVec = lexer();

    printTokens(tokenVec);

    return 0;
}