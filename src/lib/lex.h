#ifndef CALC_H
#define CALC_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct token {
    token(string data = "", int row = 0, int column = 0, string type = "") {
        this->data = data;
        this->row = row;
        this->column = column;
        this->type = type;
    }

    string data;
    int row;
    int column;
    string type;
};

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

void createTokens (const string line, const int row, vector<token> &tokenVec);

void addEndToken(vector<token> &tokenVec, const int row, const int columm);

vector<token> lexer();

void printTokens(const vector<token> tokenVec);

#endif