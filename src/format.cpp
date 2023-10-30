#include "lib/calc.h"
#include "lib/lex.h"

void indent(int indentation) {
    string spacing;
    for (int i = 0; i < indentation; i++) {
        spacing += "    ";
    }
    cout << spacing;
}

int main() {
    vector<token> tokenVec = lexer();
    int indentation = 0;
    bool elseIf = 0;
    
    for (unsigned i = 0; i < tokenVec.size(); i++) {
        if (tokenVec.at(i).data == "while") {
            indent(indentation);
            cout << "while ";
            i++;

            vector<token> expressionVec;
            while (tokenVec.at(i).data != "{") {
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }
            AST2 tree;
            token someToken;
            try {
                tree.root = build(expressionVec, someToken);
            }
            catch (error Error){
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
                exit(2);
            }
            printInfix2(tree.root);
            
            indentation++;
            cout << " {" << endl;
        }
        else if (tokenVec.at(i).data == "if") {
            indent(indentation);
            cout << "if ";
            i++;

            vector<token> expressionVec;
            while (tokenVec.at(i).data != "{") {
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }
            AST2 tree;
            token someToken;
            try {
                tree.root = build(expressionVec, someToken);
            }
            catch (error Error){
                cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
                exit(2);
            }
            printInfix2(tree.root);
            
            indentation++;
            cout << " {" << endl;
        }
        else if (tokenVec.at(i).data == "else") {
            indent(indentation);
            cout << "else ";
            i++;
            // i should either be at { or if

            indentation++;
            cout << "{" << endl;
            if (tokenVec.at(i).data == "if") {
                indent(indentation);
                cout << "if ";
                i++;

                vector<token> expressionVec;
                while (tokenVec.at(i).data != "{") {
                    expressionVec.push_back(tokenVec.at(i));
                    i++;
                }
                AST2 tree;
                token someToken;
                try {
                    tree.root = build(expressionVec, someToken);
                }
                catch (error Error){
                    cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
                    exit(2);
                }
                printInfix2(tree.root);
                
                indentation++;
                cout << " {" << endl;
                elseIf = 1;
            }
        }
        else if (tokenVec.at(i).data == "print") {
            indent(indentation);
            cout << "print ";
            i++;

            int row = tokenVec.at(i).row;
            vector<token> expressionVec; 
            while (tokenVec.at(i).row == row && tokenVec.at(i).type != "end") {
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
            printInfix2(tree.root); 
            cout << endl;
        }
        else if (tokenVec.at(i).data == "}") {
            indentation--;
            indent(indentation);
            cout << "}" << endl;
            if (elseIf == 1) {
                indentation--;
                indent(indentation);
                cout << "}" << endl;
                elseIf = 0;
            }
        }
        else { // its an expression or END or     
            if (tokenVec.at(i).type == "end") {
                return 0;
            }
            indent(indentation);
            int row = tokenVec.at(i).row;
            vector<token> expressionVec;
            while (tokenVec.at(i).row == row && tokenVec.at(i).type != "end") {
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
            printInfix2(tree.root);
            cout << endl;
        }
    }

    return 0;
}