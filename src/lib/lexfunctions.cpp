#include "lex.h"
#include <vector>
#include <iomanip>

void lexer (const string line, const int row, vector<vecComponent> &inputVec) {
    string data;
    int column = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char lineChar = line[i];

        if (isdigit(lineChar)) {
            int dotCount = 0;
            int preservedColumn = column;
            data = "";

            while (isdigit(lineChar) || lineChar == '.') {
                if (lineChar == '.') { // if has more than one '.'
                    dotCount++;
                }
                if (dotCount > 1) { // if has more than one '.'
                    cout << "Syntax error on line " << row << " column " << column << "." << endl;
                    exit(1);
                }
                data.push_back(lineChar);
                column++;
                i++;
                lineChar = line[i];
            }

            i--;

            if (line[i] == '.') { // if ends with '.'
                cout << "Syntax error on line " << row << " column " << column << "." << endl;
                exit(1);
            }
            else { // everything is good, let's create vecComponent and push onto vector
                vecComponent num;
                num.column = preservedColumn;
                num.data = data;
                num.row = row;

                inputVec.push_back(num);
            }
        }
        else if (lineChar == '(' || lineChar == ')' || lineChar == '+' || lineChar == '-' || lineChar == '*' || lineChar == '/') {   
            vecComponent op;
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

void printer(vector<vecComponent> someVec) {
    for (unsigned int i = 0; i < someVec.size(); i++) {
        cout << right << setw(4) << someVec.at(i).row << setw(5) << someVec.at(i).column << "  " << someVec.at(i).data << endl;
    }
}

void addEnd(vector<vecComponent> &inputVec, bool wasNL, int counterNL) {
    if (inputVec.size() != 0) {
        int lastRow = inputVec.back().row;
        int lastCol = inputVec.back().column;

        vecComponent endComponent;

        if (wasNL) { // newline
            endComponent.column = 1;
            endComponent.row = counterNL;
            endComponent.data = "END";
        }
        else { // no newline
            endComponent.column = lastCol + 1;
            endComponent.row = lastRow;
            endComponent.data = "END";
        }

        inputVec.push_back(endComponent);
    }
    else {
        vecComponent endComponent;
        endComponent.column = 1;
        endComponent.row = counterNL;

        inputVec.push_back(endComponent);
    }
    
}