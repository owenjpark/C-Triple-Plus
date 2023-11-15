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

        for (unsigned i = 0; i < tokenVec.size(); i++) {
            cout << tokenVec.at(i).data << endl;
        }

    //     AST2 tree;
    //     token someToken;
    //     try { // build tree
    //         tree.root = build(tokenVec, someToken);
    //     }
    //     catch (error Error){
    //         if (Error.code == 2) {
    //             cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
    //         }
    //         continue;
    //     }
        
    //     printInfix2(tree.root);
    //     cout << endl;

    //     vector<variable> temp = variables; // copy of variables vector in case of "no update on error"
    //     try { // evaluate answer
    //         boolNum result = evaluate(tree.root, temp);
    //         if (result.mType == "num") {
    //             cout << result.mNum << endl;
    //         }
    //         else { // else its a bool
    //             if (result.mBool == true) {
    //                 cout << "true" << endl;
    //             }
    //             else {
    //                 cout << "false" << endl;
    //             }
    //         }
    //     }
    //     catch (error runtime){
    //         if (runtime.code == 0) {
    //             cout << "Runtime error: division by zero."  << endl;
    //         }
    //         else if (runtime.code == 3) {
    //             cout << "Runtime error: unknown identifier " << runtime.data << endl;
    //         }
    //         else if (runtime.code == 4) {
    //             cout << "Runtime error: invalid operand type." << endl;
    //         }
    //         continue;
    //     }
    //     variables = temp; // no runtime errors, set variables to temp
    // }
   
    return 0;
}
