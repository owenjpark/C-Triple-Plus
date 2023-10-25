#include "calc.h"
#include "lex.h"

int findMatchingParenth(int i, vector<token> tokenVec) { // (12 + 7) should start at 12, returns index of )
    int parenthDiff = 1;
    while (parenthDiff != 0 && !(tokenVec.at(i).type == "end")) {
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
        if (parenthDiff == 0) { // so it doesn't over iterate
            break;
        }
        i++;
    }
    if (tokenVec.at(i).type == "end") {
        // cout << "error0" << endl;
        error someError(tokenVec.at(i).data,tokenVec.at(i).column, 2);
        throw someError;
    }

    return i;
}

void expressionChecker2(unsigned startIndex, unsigned endIndex, vector<token> tokenVec) {
    if (tokenVec.size() == 1) { // only end token
        // cout << "error1" << endl;
        error someError(tokenVec.at(0).data, tokenVec.at(0).column, 2);
        throw someError;
    }
    // has to have at least 1 real token in it
    if (tokenVec.at(startIndex).type != "num" && tokenVec.at(startIndex).type != "var" && tokenVec.at(startIndex).type != "lParenth") { // doesn't start with big 3
        // cout << "error2" << endl;
        error someError(tokenVec.at(startIndex).data, tokenVec.at(startIndex).column, 2);
        throw someError;
    }
    // at least 1 element
    for (unsigned i = startIndex; i < endIndex; i++) { // doesn't hit end token
        if (tokenVec.at(i).type == "lParenth") {
            i++; // skip "("
            int endParenthIndex = findMatchingParenth(i, tokenVec);
            expressionChecker2(i, endParenthIndex, tokenVec);
            i = endParenthIndex;
            
            if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "end") {
                // cout << "error3" << endl;
                error someError(tokenVec.at(i + 1).data, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "num" || tokenVec.at(i).type == "var") {
            if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "end") {
                // cout << "error4" << endl;
                error someError(tokenVec.at(i + 1).data, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "op") {
            cout << "yes\n";
            if (tokenVec.at(i + 1).type != "num" && tokenVec.at(i + 1).type != "var" && tokenVec.at(i + 1).type != "lParenth") {
                // cout << "error5" << endl;
                error someError(tokenVec.at(i + 1).data, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "eq") {
            if (tokenVec.at(i - 1).type != "var") {
                // cout << "error6" << endl;
                error someError(tokenVec.at(i).data, tokenVec.at(i).column, 2);
                throw someError;
            }
            if (tokenVec.at(i + 1).type != "num" && tokenVec.at(i + 1).type != "var" && tokenVec.at(i + 1).type != "lParenth") {
                // cout << "error7" << endl;
                error someError(tokenVec.at(i + 1).data, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
    }
}

