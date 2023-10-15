#include "lib/lex.h"
#include <vector>
#include <iomanip>

int main() {
    vector<vecComponent> someVec;
    string someLine;
    char someChar;
    bool wasNL;
    int counterNL;
    int counter = 1;

    while(cin.get(someChar)) {
        if (someChar == '\n') {
            lexer(someLine, counter, someVec);

            counterNL++;
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

    addEnd(someVec, wasNL, counterNL);
    
    printer(someVec);

    return 0;
}