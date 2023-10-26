#include "lex.h"
#include <iomanip>

void createTokens (string line, int row, vector<token> &inputVec) { // creates tokens by line
    string data;
    int column = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char currChar = line.at(i);

        if (isdigit(currChar)) { // start of possible number, check if valid double and push
            int dotCount = 0;
            int firstDigitColumn = column;
            data = "";

            while (isdigit(currChar) || currChar == '.') {
                if (currChar == '.') { 
                    dotCount++;
                }
                if (dotCount > 1) { // has more than 1 '.'
                    error someError(data, row, column, 1);
                    throw someError;
                }
                data.push_back(currChar);
                column++;
                i++;
                if (i < line.length()) {
                    currChar = line.at(i);
                }
                else {
                    break;
                }
            }
            i--; // last i++ redundant since for loop does it

            if (line.at(i) == '.') { // ends in '.'
                error someError(data, row, column, 1);
                throw someError;
            }
            else { // valid double, let's create token and push onto vector
                token num(data, row, firstDigitColumn, "num");
                inputVec.push_back(num);
            }
        }
        else if (isalpha(line.at(i)) || line.at(i) == '_') { // start of variable
            int firstCharColumn = column;
            data = "";

            while (isalpha(line.at(i)) || line.at(i) == '_' || isdigit(currChar)) {
                data.push_back(currChar);
                column++;
                i++;
                if (i < line.length()) {
                    currChar = line.at(i);
                }
                else {
                    break;
                }
            }
            i--; // last i++ redundant since for loop does it

            token variable (data, row, firstCharColumn, "var");       
            inputVec.push_back(variable);
        }
        else if (currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/') {   
            token op (string(1, currChar), row, column, "op"); // string(1, currChar) converts char to string bc constructor expects string
            inputVec.push_back(op);

            column++;
        }
        else if (currChar == '=') {   
            token eq (string(1, currChar), row, column, "eq");
            inputVec.push_back(eq);

            column++;
        }
        else if (currChar == '(') {
            token parenth (string(1, currChar), row, column, "lParenth");
            inputVec.push_back(parenth);

            column++;
        }
        else if (currChar == ')') {
            token parenth (string(1, currChar), row, column, "rParenth");
            inputVec.push_back(parenth);

            column++;
        }
        else if (currChar == ' ' || currChar == '\t') {
            column++;
            continue;
        }
        else {
            error someError(data, row, column, 1);
            throw someError;
        }
    }
}

void printTokens(vector<token> someVec) {
    for (unsigned int i = 0; i < someVec.size(); i++) {
        cout << right << setw(4) << someVec.at(i).row << setw(5) << someVec.at(i).column << "  " << someVec.at(i).data << endl;
    }
}

void addEndToken(vector<token> &inputVec, int row, int column) {
    token endComponent ("END", row, column, "end");
    inputVec.push_back(endComponent);
}

vector<token> lexer() { // takes in std input and returns completed vector token
    vector<token> someVec;
    string someLine;
    char someChar;
    int row = 1;
    int column = 1;

    while(cin.get(someChar)) {
        if (someChar == '\n') { // new row so create tokens, reset line, and go to next row
            createTokens(someLine, row, someVec);

            someLine = "";
            row++;
            column = 1;
        }
        else {
            someLine.push_back(someChar);
            column++;
        }
    }
    createTokens(someLine, row, someVec); // adds last row not put in by while loop
    addEndToken(someVec, row, column);

    return someVec;
}