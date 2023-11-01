#include "lib/calc.h"
#include "lib/lex.h"
#include "lib/format.h"

int main() {
    int needExit = 0;

    vector<token> tokenVec;
    try {
        tokenVec = lexer();
    }
    catch (error Error) {
        cout << "Syntax error on line " << Error.row << " column " << Error.column << "." << endl;
        exit(1);
    }
    try {
        printStatements(tokenVec);
    }
    catch (error Error) {
        cout << "Unexpected token at line " << Error.row << " column " << Error.column << ": " <<  Error.data << endl;
        needExit = 2;
    }

    if (needExit == 2) {
        exit(2);
    }

    return 0;
}