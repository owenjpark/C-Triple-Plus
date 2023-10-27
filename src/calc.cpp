#include "lib/calc.h"
#include "lib/lex.h"

int main() {
    string line;
    vector<variable> variables;

    while(std::getline(std::cin, line)) {
        vector<token> tokenVec;
        try {
            createTokens(line, 1, tokenVec);
        }
        catch (error someError){
            if (someError.code == 1) {
                cout << "Syntax error on line " << someError.row << " column " << someError.column << "." << endl;
            }
            continue;
        }
        addEndToken(tokenVec, 1, line.size() + 1);

        try{
            expressionChecker2(0, tokenVec.size() - 1, false, tokenVec);
        }
        catch(error someError) {
            if (someError.code == 2) {
                cout << "Unexpected token at line 1 " <<  "column " << someError.column << ": " << someError.data << endl;
            }
            continue;
        }

        AST2 tree;
        vector<variable> temp = variables;
        try{
            tree.root = build(tokenVec);
        }
        catch(error Error){
            if (Error.code == 2) {
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
            }
            continue;
        }
        
        string equation = infixString(tree.root);
        cout << equation << endl;

        try{
        double result =  evaluate(tree.root, temp, 0);
            cout << result << endl;
        }
        catch(error Runtime){
            if (Runtime.code == 3) {
                cout << "Runtime error: unknown identifier " << Runtime.data << endl;
            }
            else if (Runtime.code == 0) {
                cout << "Runtime error: division by zero."  << endl;
            }
            continue;
        }
        variables = temp;
    }
   
    return 0;
}