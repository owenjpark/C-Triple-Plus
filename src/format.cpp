#include "lib/calc.h"
#include "lib/lex.h"

int main() {
    vector<token> tokenVec = lexer();
    
    for (unsigned i = 0; i < tokenVec.size(); i++) {
        if (tokenVec.at(i).data == "while") {
        }
    }
}