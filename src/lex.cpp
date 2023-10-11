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

void linkedList::lexer(string line, int row) { // takes a whole line and adds to linked list
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
    int columnPreserved;
    bool firstTime = 1;

    for (unsigned int i = 0; i < line.length(); i++) {
        char lineChar = line[i];

        if ((isdigit(lineChar)) || (lineChar == '.')) {
            data.push_back(lineChar);

            if (i == line.length() - 1) { // if on last element
                Node* someNode = nullptr;
                if (firstTime == 1) {
                    someNode = createNode(data, column, row);
                    column++;
                }
                else {
                    someNode = createNode(data, columnPreserved, row);
                    column++;
                }
                
                if (currNode == nullptr) {
                    root = someNode;
                    currNode = root;
                }
                else {
                    currNode->next = someNode;
                    currNode = currNode->next;
                    data = "";
                }
                firstTime = 1;
            }
            else if (!isdigit(line[i + 1]) && line[i + 1] != '.') { // if next element is not number and '.'
                Node* someNode = nullptr;
                if (firstTime == 1) {
                    someNode = createNode(data, column, row);
                    column++;
                }
                else {
                    someNode = createNode(data, columnPreserved, row);
                    column++;
                }

                if (currNode == nullptr) {
                    root = someNode;
                    currNode = root;
                }
                else {
                    currNode->next = someNode;
                    currNode = currNode->next;
                    data = "";

                }
                firstTime = 1;
            }
            else {
                if (firstTime == 1) {
                    columnPreserved = column;
                    column++;
                    firstTime = 0;
                }
                else {
                    column++;
                    continue;
                }
            }
        }
        else if (lineChar == '(') {
            data = "(";
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
        else if (lineChar == ')') {
            data = ")";
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
        else if (lineChar == '+') {
            data = "+";
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
        else if (lineChar == '-') {
            data = "-";
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
        else if (lineChar == '*') {
            data = "*";
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
        else if (lineChar == '/') {
            data = "/";
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

    while(getline(cin, someLine)) {
        if (someLine == "\n") {
            break;
        }
        someLinkedList.lexer(someLine, counter);
        counter++;
    }

   someLinkedList.printer();
   cout << "  12    1  END" << endl;

    return 0;
}