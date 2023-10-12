#include "lib/lex.h"
#include <iomanip>

linkedList::linkedList() {
    root = nullptr;
}

linkedList::~linkedList() {
    while(root != nullptr) {
        Node* nextNode = root->next;
        delete root;
        root = nextNode;
    };
}

Node* linkedList::createNode(string data, int column, int row) {
    Node* newNode = new Node;

    newNode->data = data;
    newNode->column = column;
    newNode->row = row;

    return newNode;
}

void linkedList::lexer(const string line, const int row) { // takes a whole line and adds to linked list
    Node* currNode = root;

    if (currNode == nullptr) {
    }
    else{
        while(currNode->next != nullptr) { // sets currNode to most recent node;
        currNode = currNode->next;
        }
    }

    string data;
    int column = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char lineChar = line[i];

        if (isdigit(lineChar)) {
            int dotCount = 0;
            data = "";

            while (isdigit(lineChar) || lineChar == '.') {
                if (data.length() == 0 && lineChar == '.') {
                    cout << "Syntax error on line " << row << " column " << column << ".";
                    exit(1);
                }

                data.push_back(lineChar);
                column++;

                if (lineChar == '.') {
                    dotCount++;
                }
                if (dotCount > 1) {
                    cout << "Syntax error on line " << row << " column " << column << ".";
                    exit(1);
                }
                lineChar = line[column];
                i++;
            }
            if (line[column] == '.') {
                cout << "Syntax error on line " << row << " column " << column << ".";
                exit(1);
            }
            Node* someNode = createNode(data, column, row);
            if (currNode == nullptr) {
                    root = someNode;
                    currNode = root;
                }
            else {
                currNode->next = someNode;
                currNode = currNode->next;
                data = "";
            }
        }
        else if (lineChar == '(' || lineChar == ')' || lineChar == '+' || lineChar == '-' || lineChar == '*' || lineChar == '/') {
            data = lineChar;
            Node* someNode = createNode(data, column, row);
            column++;
            if (currNode == nullptr) {
                    root = someNode;
                    currNode = root;
                }
            else {
                currNode->next = someNode;
                currNode = currNode->next;
                data = "";
            }
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
}

void linkedList::printer() {
    Node* currNode = root;

    while (currNode != nullptr) {
        cout << right << setw(4) << currNode->row << setw(5) << currNode->column << "  " << currNode->data << endl;
        currNode = currNode->next;
    }
    
}

int main() {
    linkedList someLinkedList;
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
   someLinkedList.lexer(someLine, counter);
   someLinkedList.printer();

    return 0;
}