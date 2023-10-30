#include "lib/calc.h"
#include "lib/lex.h"

int main() {
    string line;
    vector<variable> variables;

    while(std::getline(std::cin, line)) {
        vector<token> tokenVec;
        try { // lex
            createTokens(line, 1, tokenVec);
        }
        catch (error Error){
            if (Error.code == 1) {
                cout << "Syntax error on line " << Error.row << " column " << Error.column << "." << endl;
            }
            continue;
        }
        addEndToken(tokenVec, 1, line.size() + 1);

        AST2 tree;
        vector<variable> temp = variables; // copy of variables vector in case of "no update on error"
        token someToken;
        try { // build tree
            tree.root = build(tokenVec, someToken);
        }
        catch (error Error){
            if (Error.code == 2) {
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
            }
            continue;
        }
        
        printInfix2(tree.root);
        cout << endl;

        try { // evaluate answer
            double result =  evaluate(tree.root, temp, 0);
            cout << result << endl;
        }
        catch (error runtime){
            if (runtime.code == 3) {
                cout << "Runtime error: unknown identifier " << runtime.data << endl;
            }
            else if (runtime.code == 0) {
                cout << "Runtime error: division by zero."  << endl;
            }
            continue;
        }
        variables = temp; // no runtime errors, set variables to temp
    }
   
    return 0;
}
