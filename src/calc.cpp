#include "lib/calc.h"
#include "lib/lex.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
    string line;
    while(getline(cin, line)) {
        vector<token> tokenVec;
        try {
            createTokens(line, 1, tokenVec);

        }
        catch (error someError){
            if (someError.exitCode == 1) {
                cout << "Syntax error on line " << someError.row << " column " << someError.column << "." << endl;
            }
            continue;
        }
        addEndToken(tokenVec, 1, line.size() + 1);
        // printTokens(tokenVec);
        // cout << endl;
        try{
            expressionChecker2(0, tokenVec.size() - 1, false, tokenVec);
        }
        catch(error someError) {
            if (someError.exitCode == 2) {
                cout << "Unexpected token at line 1 " <<  "column " << someError.column << ": " << someError.data << endl;
            }
            continue;
        }
    }
    return 0;
}