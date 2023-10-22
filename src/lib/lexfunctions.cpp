#include "lex.h"
#include <iomanip>

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
                num.type = "num";
                num.column = firstDigitColumn;
                num.data = data;
                num.row = row;

                inputVec.push_back(num);
            }
        }
        else if (isalpha(line.at(i)) || line.at(i) == '_') { // start of variable
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
            variable.type = "var";
            variable.column = firstCharColumn;
            variable.data = data;
            variable.row = row;

            inputVec.push_back(variable);
        }
        else if (lineChar == '+' || lineChar == '-' || lineChar == '*' || lineChar == '/') {   
            token op;
            op.type = "op";
            op.data = lineChar;
            op.column = column;
            op.row = row;

            column++;
            inputVec.push_back(op);
        }
        else if (lineChar == '=') {   
            token eq;
            eq.type = "eq";
            eq.data = lineChar;
            eq.column = column;
            eq.row = row;

            column++;
            inputVec.push_back(eq);
        }
        else if (lineChar == '(' || lineChar == ')') {
            token parenth;
            if (lineChar == '(') {
                parenth.type = "lParenth";
            }
            if (lineChar == ')') {
                parenth.type = "rParenth";
            }
            parenth.data = lineChar;
            parenth.column = column;
            parenth.row = row;

            column++;
            inputVec.push_back(parenth);
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

void addEndToken(vector<token> &inputVec, int row, int column) {
    token endComponent;
    endComponent.type = "end";
    endComponent.column = column;
    endComponent.row = row;
    endComponent.data = "END";

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