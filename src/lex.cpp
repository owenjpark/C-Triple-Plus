#include "lib/lex.h"

int main() {
    vector<vecComponent> someVec;
    string someLine = "(+(-2 4.444 )";
    char someChar;
    bool wasNL = 1;
    int counter = 1;

    while(cin.get(someChar)) {
        if (someChar == '\n') {
            lexer(someLine, counter, someVec);

            someLine = "";
            counter++;
            wasNL = 1;
        }
        else {
            someLine.push_back(someChar);
            wasNL = 0;
        }
    }

    lexer(someLine, counter, someVec);

    addEnd(someVec, wasNL);
    printer(someVec);

    return 0;
}