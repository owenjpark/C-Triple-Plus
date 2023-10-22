#include "lib/parse.h"
#include "lib/lex.h"



int main() {
    vector<token> tokenVec;

    tokenVec = lexer();

    parser(tokenVec);

    return 0;
}
