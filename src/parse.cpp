#include "lib/parse.h"
#include "lib/lex.h"



int main() {
    vector<token> tokenVec;

    tokenVec = lexer();

    // printTokens(tokenVec);

    // end of lexer

    vector <string> definedVars;

    if (tokenVec.size() == 1) { // no tokens
        cout << "Unexpected token at line " << tokenVec.at(0).row << " column " << tokenVec.at(0).column << ": " << tokenVec.at(0).data << endl;
    }

    int parenthDiff = 0;
    for (unsigned i = 0; i < tokenVec.size() - 1; i++) { // at -1 because of end token
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
    for (unsigned i = 0; i < tokenVec.size() - 1; i++) { // at -1 because of end token
        if (parenthDiff == 0) {
            AST tree;
            tree.root = createAST(tokenVec, i);
            printInfix(tree.root);
            double answer = evaluateAST(tree.root);
            cout << endl << answer << endl;
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
