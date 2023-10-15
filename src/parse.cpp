#include "lib/parse.h"
#include "lib/lex.h"
#include <vector>
#include <iomanip>

int main() {
    vector<vecComponent> someVec;
    string someLine = "(* (+ 1 2) 3 (/ 4 5 (- 6 7)))";
    // char someChar;
    bool wasNL = 1;
    int counter = 1;
    
    /*
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
    */

    lexer(someLine, counter, someVec);

    addEnd(someVec, wasNL);

    AST::node* root = parse(someVec, 0, "");

    // printer(someVec);

    someVec.pop_back();

    printEquation(root);

    cout << endl;

    cout << evaluate(root);

    return 0;
}
