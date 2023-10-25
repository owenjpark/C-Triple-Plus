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

void createTokens (const string line, const int row, vector<token> &tokenVec);

void addEndToken(vector<token> &tokenVec, int row, int columm);

vector<token> lexer();

void printTokens(vector<token> tokenVec);

struct error {
    error(string data = "", int row = 0, int column = 0, int code = 0) {
        this->data = data;
        this->row = row;
        this->column = column;
        this->code = code;
    }

    string data;
    int row;
    int column;
    int code;
};

#endif