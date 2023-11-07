#include "calc.h"
#include "lex.h"
#include "format.h"

void indent(int indentation) { // helper function for printStatements()
    string spacing;
    for (int i = 0; i < indentation; i++) {
        spacing += "    ";
    }
    cout << spacing;
}

void printStatements (vector<token> tokenVec) {
    int indentation = 0;
    bool elseIf = 0; // indicates if statement is "else if"
    bool doubleBracket = 0; // indicates we need another closing bracket because of "else if" statements
    for (unsigned i = 0; i < tokenVec.size(); i++) {
        if (tokenVec.at(i).data == "while" || tokenVec.at(i).data == "if") {
            string statement = tokenVec.at(i).data; // store statement to print later

            i++;
            vector<token> conditionVec;
            while (tokenVec.at(i).data != "{" && tokenVec.at(i).type != "condition" && tokenVec.at(i).type != "end") {
                conditionVec.push_back(tokenVec.at(i));
                i++;
            }
            AST2 tree;
            token someToken;
            tree.root = build(conditionVec, someToken);
            if (tokenVec.at(i).data != "{") {
                token errorToken = tokenVec.at(i);
                error Error(errorToken.data, errorToken.row, errorToken.column, 2);
                throw Error;
            }

            indent(indentation);
            cout << statement << " ";
            printInfix2(tree.root);
            cout << " {" << endl;

            indentation++;
        }
        else if (tokenVec.at(i).data == "else") {
            i++;
            // i should either be at { or if
            if (tokenVec.at(i).data != "{" && tokenVec.at(i).data != "if") {
                token errorToken = tokenVec.at(i);
                error Error(errorToken.data, errorToken.row, errorToken.column, 2);
                throw Error;
            }

            AST2 tree;
            token someToken;
            if (tokenVec.at(i).data == "if") {
                i++;
                vector<token> conditionVec;
                while (tokenVec.at(i).data != "{" && tokenVec.at(i).type != "condition" && tokenVec.at(i).type != "end") {
                    conditionVec.push_back(tokenVec.at(i));
                    i++;
                }
                tree.root = build(conditionVec, someToken);
                if (tokenVec.at(i).data != "{") {
                    token errorToken = tokenVec.at(i);
                    error Error(errorToken.data, errorToken.row, errorToken.column, 2);
                    throw Error;
                }
                elseIf = 1;
                indentation++;
            }
            // at index of "{"
            if (elseIf == 1) {
                indent(indentation - 1);
                cout << "else {" << endl;
                indent(indentation);
                cout << "if ";
                printInfix2(tree.root);
                cout << " {" << endl;
            }
            else { // regular else statement
                indent(indentation);
                cout << "else {" << endl;
            }

            indentation++;
        }
        else if (tokenVec.at(i).data == "print") {
            i++;
            int row = tokenVec.at(i).row;
            vector<token> outputVec; 
            while (tokenVec.at(i).row == row && tokenVec.at(i).type != "end") {
                if (tokenVec.at(i).type == "condition" && tokenVec.at(i).type == "lBracket"&& tokenVec.at(i).type == "rBracket") {
                    token errorToken = tokenVec.at(i);
                    error Error(errorToken.data, errorToken.row, errorToken.column, 2);
                    throw Error;
                }
                outputVec.push_back(tokenVec.at(i));
                i++;
            }
            i--;

            AST2 tree;
            token someToken;
            tree.root = build(outputVec, someToken);
            indent(indentation);
            cout << "print ";
            printInfix2(tree.root); 
            cout << endl;
        }
        else if (tokenVec.at(i).data == "}") {
            indentation--;
            indent(indentation);
            cout << "}" << endl;
            if (elseIf == 1) {
                if (tokenVec.at(i).type != "end") {
                    if (tokenVec.at(i + 1).data == "else") {
                        elseIf = 0;
                        doubleBracket = 1;
                        continue;
                    }
                }
                indentation--;
                indent(indentation);
                cout << "}" << endl;
                elseIf = 0;     
            }
            if (doubleBracket == 1) {
                indentation--;
                indent(indentation);
                cout << "}" << endl;
                doubleBracket = 0;
            }
        }
        else { // its an expression or END
            if (tokenVec.at(i).type == "end") {
                return;
            }
    
            int row = tokenVec.at(i).row;
            vector<token> expressionVec;
            while (tokenVec.at(i).row == row && tokenVec.at(i).type != "end") {
                if (tokenVec.at(i).type == "condition" && tokenVec.at(i).type == "lBracket" && tokenVec.at(i).type == "rBracket") {
                    token errorToken = tokenVec.at(i);
                    error Error(errorToken.data, errorToken.row, errorToken.column, 2);
                    throw Error;
                }
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }
            i--;

            AST2 tree;
            token someToken;
            tree.root = build(expressionVec, someToken);
            indent(indentation);
            printInfix2(tree.root);
            cout << endl;
        }
    }
}

