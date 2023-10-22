#include "lib/calc.h"
#include "lib/lex.h"
#include <iostream>
#include <string>
#include <vector>




AST::AST(){
    root = nullptr;
}

AST::~AST(){
  destructor(root);
}

void AST::destructor(AST::Node* node){
    
    if (node->leftChild != nullptr) {
        destructor(node->leftChild);
    }

    else if (node->rightChild != nullptr) {
        destructor(node->rightChild);
    }
    // base case if it has no children
    else {
        delete node;
    }

    
}


// helper function for build 
//should be working but have to add symbols and equal sign 
int precedence(vector<token> vec) {
    
    //  1 for + -, 2 for * /, 3 for () and numbers
    // grab the column and data for the operator with the least precedence
   int rating = 10;
   int opLeast; 
   int temp = 1;
   string first = vec[0].data;

   int i = 0;
   int size = vec.size();
   

    while (i < size) {
        if (vec[i].data == "=") temp = 0;
        else if (vec[i].data == "+" || vec[i].data == "-") {
            temp = 1;
        }

        else if( vec[i].data == "*" || vec[i].data == "/") {
            temp = 2;
        }

        else if(vec[i].data == "(") {
            temp = 3; 
            // going to the index )
            while (vec[i].data != ")") { 
                i++;
            }
            
        }
        // if its a number 
        else {
            temp = 4;
        }

        if (temp <= rating) {
            rating = temp;
            opLeast = i;
        }

        i++;

    }
    return opLeast;
}

bool isFloat(string someString) { // helps for check and build
    for (unsigned i = 0; i < someString.size(); i++) {
        if (isdigit(someString.at(i)) || someString.at(i) == '.') {
            continue;
        }
        else {
            return false;
        }  
    }
    return true;
}

AST::Node* build(vector<token> vec) {
    // assumming there are no parse errors going in 
    // right now only the operators and parenthesis

    // call precedence and make that the node data 
    // store index of least 
    // base case vec.size() = 1 or 0
    if (vec.size() == 0) return nullptr;

    // base case: if the vector is only a num or variable
    if (vec.size() == 1) {
        AST::Node* num = new AST::Node();
        num->data = vec.at(0).data;
        return num;
    }

    // case if argument is inside ()
        if ( vec.at(0).data == "(") {
        if (vec.at(vec.size()).data == ")" || vec.at(vec.size()).data == "END") {
        int lcount;
        int rcount;
        int stop = vec.size() -1;
        if (vec.at(vec.size()).data == "END") {
            stop = stop - 2;}
        for (int j=0; j <= stop; j++) {
            if (vec.at(0).data == "(") lcount += 1;
            if (vec.at(0).data == ")") rcount += 1;
            if (rcount > lcount) {
                // output error
            }
            if (rcount != lcount) {
                //output error
            }

            //checking (..) is the only argument
            if (rcount == 1) {
                // remove the parenthesis from the vector
                vec.erase(vec.begin());
                vec.erase(vec.begin() + stop);
            }
        }
        }
        else {
            //output error
        }
    }


    int low = 0; // index of lowest precedence operation
    AST::Node* oper = new AST::Node();

    low = precedence(vec);
    oper->data = vec.at(low).data;

    // then call build with left side vec[0] - vec[least-1]        <-- leftchild points to result
    
    vector<token> leftVec;
    int j;
    for (j; j < low; j++) {
        leftVec.push_back(vec[j]);
    }
    oper->leftChild = (build(leftVec));
   
    // then call precedence of right side vec[least + 1] vec.size() - 1  <-- right child points to result 
    vector<token> rightVec;
    int i = low + 1;
    for (i; i < vec.size(); i++) {
        rightVec.push_back(vec[i]);
    }
    oper->rightChild = (build(rightVec));
    
}


// will cout the output in main
string stringAST(AST::Node* root) {

}

//
float evaluate(AST::Node* root){ 

}


int main() {
    vector<token> tokenVec;
    tokenVec = lexer();

    AST tree;
    tree.root = build(tokenVec);

    
    return 0;
}