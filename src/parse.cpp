#include "lib/parse.h"
#include "lib/lex.h"
#include <vector>
#include <iomanip>

int main() {
    vector<vecComponent> someVec;
    string someLine;
    char someChar;
    bool wasNL;
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

    // printer(someVec);

    AST::node* root = parse(someVec, 0); // seg fault here

    printEquation(root);

    cout << endl;

    cout << evaluate(root);

    return 0;
}
