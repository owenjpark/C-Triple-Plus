#include "lib/parse.h"
#include "lib/lex.h"



int main() {
    vector<token> tokenVec;

    tokenVec = lexer();

    // end of lexer

    expressionChecker(tokenVec);

    AST someAST;
    someAST.root = createAST(tokenVec, 0);
    printInfix(someAST.root);

    double result = evaluateAST(someAST.root); // in case divided by 0

    cout << endl;

    cout << result;

    cout << endl;

    return 0;
}
