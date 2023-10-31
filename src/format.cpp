#include "lib/calc.h"
#include "lib/lex.h"

using namespace std; // TODO delete this

void indent(int indentation) {
    string spacing;
    for (int i = 0; i < indentation; i++) {
        spacing += "    ";
    }
    cout << spacing;
}

int main() {
    vector<token> tokenVec;
    try {
        tokenVec = lexer();
    }
    catch (error Error) {
        cout << "Syntax error on line " << Error.row << " column " << Error.column << "." << endl;
        exit(1);
    }
    
    int indentation = 0;
    bool elseIf = 0;
    for (unsigned i = 0; i < tokenVec.size(); i++) {
        if (tokenVec.at(i).data == "while") {
            i++;
            vector<token> expressionVec;
            while (tokenVec.at(i).data != "{" && tokenVec.at(i).type != "condition" && tokenVec.at(i).type != "end") {
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }
            AST2 tree;
            token someToken;
            try {  
                tree.root = build(expressionVec, someToken);
            }
            catch (error Error){
                cout << "test2" << endl;
                cout << "Unexpected token at line " << Error.row << " column " << Error.column << ": " << Error.data << endl;
                exit(2);
            }
            if (tokenVec.at(i).data != "{") {
                cout << "test1" << endl;
                cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " <<  tokenVec.at(i).data << endl;
                exit(2);
            }

            indent(indentation);
            cout << "while ";
            printInfix2(tree.root);
            cout << " {" << endl;

            indentation++;
        }
        else if (tokenVec.at(i).data == "if") {
            i++;
            vector<token> expressionVec;
            while (tokenVec.at(i).data != "{" && tokenVec.at(i).type != "condition" && tokenVec.at(i).type != "end") {
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }
            AST2 tree;
            token someToken;
            try {  
                tree.root = build(expressionVec, someToken);
            }
            catch (error Error){
                cout << "Unexpected token at line " << Error.row << " column " << Error.column << ": " << Error.data << endl;
                exit(2);
            }
            if (tokenVec.at(i).data != "{") {
                cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " <<  tokenVec.at(i).data << endl;
                exit(2);
            }

            indent(indentation);
            cout << "if ";
            printInfix2(tree.root);
            cout << " {" << endl;

            indentation++;
        }
        else if (tokenVec.at(i).data == "else") {
            i++;
            // i should either be at { or if
            if (tokenVec.at(i).data != "{" && tokenVec.at(i).data != "if") {
                cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " <<  tokenVec.at(i).data << endl;
                exit(2);
            }

            AST2 tree;
            token someToken;
            if (tokenVec.at(i).data == "if") {
                i++;
                vector<token> expressionVec;
                while (tokenVec.at(i).data != "{" && tokenVec.at(i).type != "condition" && tokenVec.at(i).type != "end") {
                    expressionVec.push_back(tokenVec.at(i));
                    i++;
                }
                try {  
                    tree.root = build(expressionVec, someToken);
                }
                catch (error Error){
                    cout << "Unexpected token at line " << Error.row << " column " << Error.column << ": " << Error.data << endl;
                    exit(2);
                }
                if (tokenVec.at(i).data != "{") {
                    cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " <<  tokenVec.at(i).data << endl;
                    exit(2);
                }
                elseIf = 1;
                indentation++;
            }
            if (elseIf == 1) {
                indent(indentation - 1);
                cout << "else {" << endl;
                indent(indentation);
                cout << "if ";
                printInfix2(tree.root); // seg fault
                cout << " {" << endl;
            }
            else {
                indent(indentation);
                cout << "else {" << endl;
            }

            indentation++;
        }
        else if (tokenVec.at(i).data == "print") {
            i++;
            int row = tokenVec.at(i).row;
            vector<token> expressionVec; 
            while (tokenVec.at(i).row == row && tokenVec.at(i).type != "end") {
                if (tokenVec.at(i).type == "condition" && tokenVec.at(i).type == "lBracket"&& tokenVec.at(i).type == "rBracket") {
                    cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " <<  tokenVec.at(i).data << endl;
                    exit(2);
                }
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }
            i--;

            AST2 tree;
            token someToken;
            try {
                tree.root = build(expressionVec, someToken);
            }
            catch (error Error){
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
                exit(2);
            }
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
                        continue;
                    }
                }
                indentation--;
                indent(indentation);
                cout << "}" << endl;
                elseIf = 0;     
            }
        }
        else { // its an expression or END
            if (tokenVec.at(i).type == "end") {
                return 0;
            }
    
            int row = tokenVec.at(i).row;
            vector<token> expressionVec;
            while (tokenVec.at(i).row == row && tokenVec.at(i).type != "end") {
                if (tokenVec.at(i).type == "condition" && tokenVec.at(i).type == "lBracket" && tokenVec.at(i).type == "rBracket") {
                    cout << "Unexpected token at line " << tokenVec.at(i).row << " column " << tokenVec.at(i).column << ": " <<  tokenVec.at(i).data << endl;
                    exit(2);
                }
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }
            i--;

            AST2 tree;
            token someToken;
            try {
                tree.root = build(expressionVec, someToken);
            }
            catch (error Error){
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
                exit(2);
            }
            indent(indentation);
            printInfix2(tree.root);
            cout << endl;
        }
    }

    return 0;
}