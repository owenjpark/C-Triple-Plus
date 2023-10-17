#include "lex.h"
#include <iomanip>

void createTokens (const string line, const int row, vector<token> &tokenVec) { // creates tokens by row
    string data = "";
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
                lineChar = line.at(i);
            }
            i--; // last i++ redundant because for loop automatically does it

            if (line.at(i) == '.') {
                cout << "Syntax error on line " << row << " column " << column << "." << endl;
                exit(1);
            }
            else { // valid double
                token num;
                num.column = firstDigitColumn;
                num.data = data;
                num.row = row;

                tokenVec.push_back(num);
            }
        }
        else if (lineChar == '(' || lineChar == ')' || lineChar == '+' || lineChar == '-' || lineChar == '*' || lineChar == '/') {   
            token op;
            op.data = lineChar;
            op.column = column;
            op.row = row;

            column++;
            tokenVec.push_back(op);
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

void printTokens(vector<token> tokenVec) {
    for (unsigned int i = 0; i < tokenVec.size(); i++) {
        cout << right << setw(4) << tokenVec.at(i).row << setw(5) << tokenVec.at(i).column << "  " << tokenVec.at(i).data << endl;
    }
}

void addEndToken(vector<token> &tokenVec, bool wasNewLine, int newlineCounter) {
    if (tokenVec.size() != 0) { // there is at least 1 token
        int lastRow = tokenVec.back().row;
        int lastCol = tokenVec.back().column;

        token endToken;

        if (wasNewLine) { // input ends with newline
            endToken.column = 1;
            endToken.row = newlineCounter + 1;
            endToken.data = "END";
        }
        else { // input doesn't end with newline
            endToken.column = lastCol + 1;
            endToken.row = lastRow;
            endToken.data = "END";
        }

        tokenVec.push_back(endToken);
    }
    else { // no tokens
        token endToken;
        endToken.column = 1;
        endToken.row = newlineCounter + 1;
        endToken.data = "END";

        tokenVec.push_back(endToken);
    }
}

vector<token> lexer() { // takes in standard input and returns completed token vector
    vector<token> tokenVec;
    string line = "";
    char someChar;
    bool wasNewLine; // see if last char in input was newline; used to create end token with correct row #
    int newlineCounter = 0; // counts the amount of newlines; used to create end token with correct row #
    int row = 1;

    while(cin.get(someChar)) {
        if (someChar == '\n') { // new row so create tokens, reset line, and go to next row
            createTokens(line, row, tokenVec);

            newlineCounter++;
            line = "";
            row++;
            wasNewLine = 1;
        }
        else {
            line.push_back(someChar);
            wasNewLine = 0;
        }
    }
    createTokens(line, row, tokenVec); // adds last row not put in by while loop

    addEndToken(tokenVec, wasNewLine, newlineCounter);

    return tokenVec;
}