#include "lex.h"
#include <iostream>
#include <string>
#include <vector>

struct error {
    error(string data, int column, int exitCode) {
        this->data = data;
        this->column = column;
        this->exitCode = exitCode;
    }

    string data;
    int column;
    int exitCode;
}; 

void expressionChecker2(unsigned startIndex, unsigned endIndex, vector<token> tokenVec);

    