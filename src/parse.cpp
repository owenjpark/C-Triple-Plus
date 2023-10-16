#include "lib/parse.h"
#include "lib/lex.h"

int main() {
    vector<token> tokenVec;
    tokenVec = lexer();

    // end of lexer

    AST expressionTree;
    parser(tokenVec);

    printInfix(expressionTree.root);

    double result = evaluateAST(expressionTree.root); // in case divided by 0 need to throw error without printing

    cout << endl;

    cout << result;

    cout << endl;

    return 0;
}
