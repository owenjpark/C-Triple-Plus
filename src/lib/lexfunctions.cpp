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
        else if (isalpha(line.at(i)) || line.at(i) == '_') { // start of variable, bool, conditional, or print
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

            // check if not variable
            if (data == "true") {
                token boolTrue (data, row, firstCharColumn, "bool");       
                inputVec.push_back(boolTrue);
                continue;
            }
            else if (data == "false") {
                token boolFalse (data, row, firstCharColumn, "bool");       
                inputVec.push_back(boolFalse);
                continue;
            }
            else if (data == "if") {
                token ifCondition (data, row, firstCharColumn, "condition");
                inputVec.push_back(ifCondition);
                continue;
            }
            else if (data == "else") {
                token elseCondition (data, row, firstCharColumn, "condition");
                inputVec.push_back(elseCondition);
                continue;
            }
            else if (data == "while") {
                token whileCondition (data, row, firstCharColumn, "condition"); 
                inputVec.push_back(whileCondition);
                continue;
            }
            else if (data == "print") {
                token printFunc (data, row, firstCharColumn, "print");       
                inputVec.push_back(printFunc);
                continue;
            }

            token variable (data, row, firstCharColumn, "var");       
            inputVec.push_back(variable);
        }
        else if (currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/' || currChar == '%') {   
            token op (string(1, currChar), row, column, "op"); // string(1, currChar) converts char to string bc constructor expects string
            inputVec.push_back(op);

            column++;
        }
        else if (currChar == '=') {
            if (i < line.size() - 1) { // prevents seg fault
                if (line.at(i + 1) == '=') {
                    token doubleEq("==", row, column, "eqIneq");
                    inputVec.push_back(doubleEq);
                    column++;
                    column++;
                    i++;
                    continue;
                }
            }
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
        else if (currChar == '&' || currChar == '^' || currChar == '|') {
            token logicOp (string(1, currChar), row, column, "logicOp");
            inputVec.push_back(logicOp);

            column++;
        }
        else if (currChar == '{') {
            token bracket (string(1, currChar), row, column, "lBracket");
            inputVec.push_back(bracket);

            column++;
        }
        else if (currChar == '}') {
            token bracket (string(1, currChar), row, column, "rBracket");
            inputVec.push_back(bracket);

            column++;
        }
        else if (currChar == ',') {
            token comma (string(1, currChar), row, column, "comma");
        }
        else if (currChar == ';') {
            token semi (string(1, currChar), row, column, "semi");
        }
        else if (currChar == '!') {
            if (i < line.size() - 1) { // prevents seg fault
                if (line.at(i + 1) == '=') {
                    token notEq("!=", row, column, "eqIneq");
                    inputVec.push_back(notEq);
                    column++;
                    column++;
                    i++;
                    continue;
                }
            }
            error someError(data, row, column + 1, 1);
            throw someError;
        }
        else if (currChar == '<') {
            if (i < line.size() - 1) { // prevents seg fault
                if (line.at(i + 1) == '=') {
                    token lessEq("<=", row, column, "op");
                    inputVec.push_back(lessEq);
                    column++;
                    column++;
                    i++;
                    continue;
                }
            }
            token less (string(1, currChar), row, column, "op");
            inputVec.push_back(less);

            column++;
        }
        else if (currChar == '>') {
            if (i < line.size() - 1) { // prevents seg fault
                if (line.at(i + 1) == '=') {
                    token greaterEq(">=", row, column, "op");
                    inputVec.push_back(greaterEq);
                    column++;
                    column++;
                    i++;
                    continue;
                }
            }
            token greater (string(1, currChar), row, column, "op");
            inputVec.push_back(greater);

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
    char currChar;
    int row = 1;
    int column = 1;

    while(cin.get(currChar)) {
        if (currChar == '\n') { // new row so create tokens, reset line, and go to next row
            createTokens(someLine, row, someVec);

            someLine = "";
            row++;
            column = 1;
        }
        else {
            someLine.push_back(currChar);
            column++;
        }
    }
    createTokens(someLine, row, someVec); // adds last row not put in by while loop
    addEndToken(someVec, row, column);

    return someVec;
}