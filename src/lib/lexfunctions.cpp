#include "lex.h"
#include <iomanip>

void createTokens (const string line, const int row, vector<token> &inputVec) {
    string data;
    int column = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char lineChar = line[i];

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
                lineChar = line[i];
            }
            i--; // last i++ redundant since for loop does it

            if (line[i] == '.') {
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

        if (wasNewLine) { // last char was "\n"
            endComponent.column = 1;
            endComponent.row = newlineCounter + 1;
            endComponent.data = "END";
        }
        else {
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

vector<token> lexer() {
    vector<token> someVec;
    string someLine;
    char someChar;
    bool wasNewLine;
    int newlineCounter = 0;
    int row = 1;

    while(cin.get(someChar)) {
        if (someChar == '\n') {
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

    createTokens(someLine, row, someVec);
    addEndToken(someVec, wasNewLine, newlineCounter);

    return someVec;
}