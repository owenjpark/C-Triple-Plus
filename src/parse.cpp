#include "lib/parse.h"
#include "lib/lex.h"
#include <vector>
#include <iomanip>


int main() {
    vector<token> tokenVec;

    tokenVec = lexer();

    // end of lexer

    validCheck(tokenVec);

    AST someAST;
    someAST.root = parse(tokenVec, 0);
    printEquation(someAST.root);

    double result = evaluate(someAST.root); // in case divided by 0

    cout << endl;

    cout << result;

    cout << endl;

    return 0;
}
