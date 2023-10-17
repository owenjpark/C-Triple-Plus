#ifndef CALC_H
#define CALC_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct token {
    string data;
    int column;
    int row;

};

void createTokens (const string line, const int row, vector<token> &tokenVec);

void addEndToken(vector<token> &tokenVec, bool wasNL, int counterNL);

vector<token> lexer();

void printTokens(vector<token> tokenVec);

#endif