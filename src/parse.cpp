#include "lib/AST.h"

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

    AST::node* root = parse(someVec, 0, "");

    printEquation(root);

    cout << endl;

    cout << evaluate(root) << endl;

    return 0;
}