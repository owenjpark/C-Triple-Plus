#include "lib/parse.h"
#include "lib/lex.h"



int main() {
    vector<token> tokenVec;

    try {
        tokenVec = lexer();
    }
    catch (int exitCode){
        exit(exitCode);
    }

    parser(tokenVec);

    return 0;
}
