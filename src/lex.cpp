#include "lib/lex.h"
#include <vector>
#include <iomanip>

void addEnd(vector<vecComponent> &inputVec) {
    int row = inputVec.back().row + 1;

    vecComponent endComponent;

    endComponent.column = 1;
    endComponent.row = row;
    endComponent.data = "END";

    inputVec.push_back(endComponent);
}

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
                cout << "char is " << lineChar << endl;
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
        else if (lineChar == ' ') {
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

int main() {
    vector<vecComponent> someVec;
    string someLine;
    int counter = 1;


    while(getline(cin, someLine)) {
        if (someLine == "\n") {
            break;
        }
        lexer(someLine, counter, someVec);
        counter++;
    }
    
    
    
    // someLine = "1()1.0123+1+0.01";
    // lexer(someLine, counter, someVec);
    
    addEnd(someVec);
    printer(someVec);

    return 0;
}