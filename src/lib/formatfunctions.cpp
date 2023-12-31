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
            tree.root = build(conditionVec);

            indent(indentation);
            cout << statement << " ";
            printInfix(tree.root);
            cout << " {" << endl;

            indentation++;
        }
        else if (tokenVec.at(i).type == "def" || tokenVec.at(i).type == "name"){
            indent(indentation); // fixing indentation
            string type;
            if (tokenVec.at(i).type == "def") {
                cout << "def "; 
                type = "def";
                i++; // going to function name 
            }
            else {
                type = "name";
            }
            cout << tokenVec.at(i).data;

            // going to identifiers starting at (
            i++;
            while (i < tokenVec.size() && tokenVec.at(i).data != ")") {
                if (tokenVec.at(i).data == ","){
                    cout << ", ";
                }
                else {
                    cout << tokenVec.at(i).data;
                }
                i++;
            }
            // printing the )
            cout << tokenVec.at(i).data;
            if (type == "def") {
                i++;
                //printing the {
                cout << " " << tokenVec.at(i).data << endl;
                indentation++;
            }
            else {
                cout <<";" << endl;
                // getting past ; for calls 
                 i++;
            }
            
        }
        else if (tokenVec.at(i).data == "else") {
            i++;
            // i should either be at { or if

            AST2 tree;
            if (tokenVec.at(i).data == "if") {
                i++;
                vector<token> conditionVec;
                while (tokenVec.at(i).data != "{" && tokenVec.at(i).type != "condition" && tokenVec.at(i).type != "end") {
                    conditionVec.push_back(tokenVec.at(i));
                    i++;
                }
                tree.root = build(conditionVec);
                elseIf = 1;
                indentation++;
            }
            // at index of "{"
            if (elseIf == 1) {
                indent(indentation - 1);
                cout << "else {" << endl;
                indent(indentation);
                cout << "if ";
                printInfix(tree.root);
                cout << " {" << endl;
            }
            else { // regular else statement
                indent(indentation);
                cout << "else {" << endl;
            }

            indentation++;
        }
        else if (tokenVec.at(i).data == "print" || tokenVec.at(i).data == "return") {
            string task = tokenVec.at(i).data;
            i++;
            vector<token> outputVec; 
            while (tokenVec.at(i).data != ";" && tokenVec.at(i).type != "end") {
                outputVec.push_back(tokenVec.at(i));
                i++;
            }

            AST2 tree;

            if (outputVec.size() != 0) {
                tree.root = build(outputVec);
            }
            indent(indentation);
            if (task == "print"){
                cout << "print ";
                printInfix(tree.root);
            }
            else {
                if (outputVec.size() != 0) {
                    cout << "return ";
                    printInfix(tree.root); 
                }
                else cout << "return";
            }
            cout << ";" << endl;
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
            // its an expression
    
            vector<token> expressionVec;
            while (tokenVec.at(i).data != ";" && tokenVec.at(i).type != "end") {
                expressionVec.push_back(tokenVec.at(i));
                i++;
            }

            AST2 tree;
            tree.root = build(expressionVec);
            indent(indentation);
            printInfix(tree.root);
            cout << ";" << endl;
        }
    }
}

