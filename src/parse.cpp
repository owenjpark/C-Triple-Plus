#include "lib/parse.h"
#include "lib/lex.h"



int main() {
    vector<token> tokenVec;

    tokenVec = lexer();

    printTokens(tokenVec);

    // end of lexer

    vector <string> definedVars;

    expressionChecker(0, tokenVec, definedVars);
    int parenthDiff = 1;
    for (unsigned i = 1; i < tokenVec.size() - 1; i++) { // at -1 because of end token
        if (parenthDiff == 0) {
            expressionChecker(i, tokenVec, definedVars);
        }
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
    }

    /* AST expressionTree = parser(tokenVec);

    printInfix(expressionTree.root);

    double result = evaluateAST(expressionTree.root); // in case divided by 0 we need to throw exitcode before printing result

    cout << endl;

    cout << result;

    cout << endl; */

    return 0;
}
