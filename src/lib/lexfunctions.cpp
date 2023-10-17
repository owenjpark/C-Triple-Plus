#include "lex.h"
#include <iomanip>

void createTokens (const string line, const int row, vector<token> &inputVec) {
    string data;
    int column = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char lineChar = line.at(i);

        if (isdigit(lineChar)) { // character is number, so lets check if it's a valid double
            int dotCount = 0;
            int firstDigitColumn = column;
            data = "";

            while (isdigit(lineChar) || lineChar == '.') {
                if (lineChar == '.') { 
                    dotCount++;
                }
                if (dotCount > 1) { 
                    cout << "Syntax error on line " << row << " column " << column << "." << endl;
                    exit(1);
                }
                data.push_back(lineChar);
                column++;
                i++;
                if (i < line.length()) {
                    lineChar = line.at(i);
                }
                else {
                    break;
                }
            }
            i--; // last i++ redundant since for loop does it

            if (line.at(i) == '.') {
                cout << "Syntax error on line " << row << " column " << column << "." << endl;
                exit(1);
            }
            else { // valid double, let's create token and push onto vector
                token num;
                num.column = firstDigitColumn;
                num.data = data;
                num.row = row;

                inputVec.push_back(num);
            }
        }
        if (isalpha(line.at(i)) || line.at(i) == '_') { // start of variable
            int firstCharColumn = column;
            data = "";

            while (isalpha(line.at(i)) || line.at(i) == '_' || isdigit(lineChar)) {
                data.push_back(lineChar);
                column++;
                i++;
                if (i < line.length()) {
                    lineChar = line.at(i);
                }
                else {
                    break;
                }
            }
            i--; // last i++ redundant since for loop does it
            token variable;
            variable.column = firstCharColumn;
            variable.data = data;
            variable.row = row;

            inputVec.push_back(variable));
        }
        else if (lineChar == '(' || lineChar == ')' || lineChar == '+' || lineChar == '-' || lineChar == '*' || lineChar == '/') {   
            token op;
            op.data = lineChar;
            op.column = column;
            op.row = row;

            column++;
            inputVec.push_back(op);
        }
        else if (lineChar == ' ' || lineChar == '\t') {
            column++;
            continue;
        }
        else {
            cout << "Syntax error on line " << row << " column " << column << "." << endl;
            exit(1);
        }
    }
}

void printTokens(vector<token> someVec) {
    for (unsigned int i = 0; i < someVec.size(); i++) {
        cout << right << setw(4) << someVec.at(i).row << setw(5) << someVec.at(i).column << "  " << someVec.at(i).data << endl;
    }
}

void addEndToken(vector<token> &inputVec, bool wasNewLine, int newlineCounter) {
    if (inputVec.size() != 0) {
        int lastRow = inputVec.back().row;
        int lastCol = inputVec.back().column;

        token endComponent;

        if (wasNewLine) { // input ends with newline
            endComponent.column = 1;
            endComponent.row = newlineCounter + 1;
            endComponent.data = "END";
        }
        else { // input doesn't end with newline
            endComponent.column = lastCol + 1;
            endComponent.row = lastRow;
            endComponent.data = "END";
        }

        inputVec.push_back(endComponent);
    }
    else {
        token endComponent;
        endComponent.column = 1;
        endComponent.row = newlineCounter + 1;
        endComponent.data = "END";

        inputVec.push_back(endComponent);
    }
}

vector<token> lexer() { // takes in std input and returns completed vector token
    vector<token> someVec;
    string someLine;
    char someChar;
    bool wasNewLine; // see if last char in input was newline; used to create end token with correct row & column
    int newlineCounter = 0; // used to create end token with correct row & column
    int row = 1;

    while(cin.get(someChar)) {
        if (someChar == '\n') { // new row so create tokens, reset line, and go to next row
            createTokens(someLine, row, someVec);

            newlineCounter++;
            someLine = "";
            row++;
            wasNewLine = 1;
        }
        else {
            someLine.push_back(someChar);
            wasNewLine = 0;
        }
    }
    createTokens(someLine, row, someVec); // adds last row not put in by while loop

    addEndToken(someVec, wasNewLine, newlineCounter);

    return someVec;
}