#ifndef CALC_H
#define CALC_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

struct vecComponent {
    string data = 0;
    int column = 0;
    int row = 0;

};

void lexer (const string line, const int row, vector<vecComponent> &inputVec);

void printer(vector<vecComponent> someVec);

void addEnd(vector<vecComponent> &inputVec, bool wasNL);


#endif