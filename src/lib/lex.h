#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct vecComponent {
    string data;
    int column;
    int row;

};

void lexer (const string line, const int row, vector<vecComponent> &inputVec);

void printer(vector<vecComponent> someVec);

void addEnd(vector<vecComponent> &inputVec, bool wasNL);


#endif