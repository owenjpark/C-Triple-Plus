#include "lib/lex.h"
#include <vector>
#include <iomanip>

vector<vecComponent> lexer (const string line, const int row) {
    vector<vecComponent> returnVec;
    string data;
    int column = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char lineChar = line[i];

        if (isdigit(lineChar)) {
            int dotCount = 0;
            int preservedColumn = column;
            data = "";

            while (isdigit(lineChar) || lineChar == '.') {
                if (data.length() == 0 && lineChar == '.') { // if begins with '.'
                    cout << "Syntax error on line " << row << " column " << column << ".";
                    exit(1);
                }
                if (lineChar == '.') { // if has more than one '.'
                    dotCount++;
                }
                if (dotCount > 1) { // if has more than one '.'
                    cout << "Syntax error on line " << row << " column " << column << ".";
                    exit(1);
                }
                data.push_back(lineChar);
                column++;
                i++;
                lineChar = line[i];
            }
            if (line[i] == '.') { // if ends with '.'
                cout << "Syntax error on line " << row << " column " << column << ".";
                exit(1);
            }
            else { // everything is good, let's create vecComponent and push onto vector
                vecComponent num;
                num.column = preservedColumn;
                num.data = data;
                num.row = row;

                returnVec.push_back(num);
            }

            i--;
        }
        else if (lineChar == '(' || lineChar == ')' || lineChar == '+' || lineChar == '-' || lineChar == '*' || lineChar == '/') {   
            vecComponent op;
            op.data = lineChar;
            op.column = column;
            op.row = row;

            column++;
            returnVec.push_back(op);
        }
        else if (lineChar == ' ') {
            column++;
            continue;
        }
        else {
            cout << "Syntax error on line " << row << " column " << column << ".";
            exit(1);
        }
    }

    return returnVec;
}

void printer(vector<vecComponent> someVec) {
    for (unsigned int i = 0; i < someVec.size(); i++) {
        cout << someVec.at(i).row << "\t" << someVec.at(i).column << "\t" << someVec.at(i).data << "\t" << endl;
    }
}

int main() {
    string someLine;
    int counter = 1;

    /*
    while(getline(cin, someLine)) {
        if (someLine == "\n") {
            break;
        }
        someLinkedList.lexer(someLine, counter);
        counter++;
    }
    */

   someLine = "(+(-2 4.444 )";
   printer(lexer(someLine, counter));

    return 0;
}