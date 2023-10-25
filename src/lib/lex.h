#ifndef CALC_H
#define CALC_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct token {
    string type;
    
    string data;
    int column;
    int row;

};

struct error {
    error(string data, int row, int column, int exitCode) {
        this->data = data;
        this->row = row;
        this->column = column;
        this->exitCode = exitCode;
    }

    string data;
    int row;
    int column;
    int exitCode;
};

void createTokens (const string line, const int row, vector<token> &tokenVec);

void addEndToken(vector<token> &tokenVec, int row, int columm);

vector<token> lexer();

void printTokens(vector<token> tokenVec);

#endif