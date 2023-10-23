#include "lib/calc.h"
#include "lib/lex.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

AST::AST() {
    root = nullptr;
};

AST::~AST() {
    clear(root);
}

void AST::clear (Node* node){
    if (node == nullptr) return;
    if (node->leftChild != nullptr) {
        clear(node->leftChild);
    }

    if (node->rightChild != nullptr) {
        clear(node->rightChild);
    }
    // base case if it has no children
    delete node;

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
        // if its a number or variable
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


AST::Node* build(vector<token> vec) {
    // assumming there are no parse errors going in 
    // right now only the operators and parenthesis

    // call precedence and make that the node data 
    // store index of least 
    // base case vec.size() = 1 or 0
    AST::Node* error = new AST::Node(); 
    //error->data = "ERROR";
    if (vec.size() == 0) return nullptr;

    if (vec.at(vec.size() - 1).data == "END") vec.pop_back();;
    

    if (vec.size() == 0) return nullptr;

    // base case: if the vector is only a num or variable
    if (vec.size() == 1) {
        AST::Node* num = new AST::Node();
        num->data = vec.at(0).data;
        num->type = vec.at(0).type;
        return num;
    }

    // case if argument is inside ()
    if ( vec.at(0).data == "(") {
        if (vec.at(vec.size()- 1 ).data == ")" ) {// && vec.at(vec.size() - 1).data == "END" ) ) {
        int count = 0;
        //int count = 0;
        bool nested = true;
        /*if (vec.at(vec.size()-1).data == "END") {
            stop = stop - 1;
            endToken = true;
            } */
        for (int j=0; j < int(vec.size()); j++) {
            if (vec.at(j).data == "(") count += 1;
            if (vec.at(j).data == ")") count -= 1;
            if (count < 0) {
                 cout << "Unexpected token at line "<< vec.at(j).row <<" column " << vec.at(j).column << ": )" << endl;
                 //return error;
            }
            if (count == 0 && j != (vec.size() - 1) ) nested = false;
            
        }
        if (count != 0) {
            cout << "BUnexpected token at line "<< vec.at(vec.size()- 1).row <<" column " << vec.at(vec.size() - 1).column << ": END" << endl;
            //return error;
        }
        
        if (nested) {
        vec.erase(vec.begin());
        vec.pop_back();
        }
            
    } 
    else {
            cout << "CUnexpected token at line "<< vec.at(vec.size()- 1).row <<" column " << vec.at(vec.size() - 1).column << ": END" << endl;
            //return error;
        }
    }


    int low = 0; // index of lowest precedence operation
    AST::Node* oper = new AST::Node();

    low = precedence(vec);
    oper->data = vec.at(low).data;
    oper->type = vec.at(low).type;

    // then call build with left side vec[0] - vec[least-1]        <-- leftchild points to result
    
    vector<token> leftVec;
    int j=0;
    for (j; j < low; j++) {
        leftVec.push_back(vec[j]);
    }
    oper->leftChild = (build(leftVec));
   
    // then call precedence of right side vec[least + 1] vec.size() - 1  <-- right child points to result 
    vector<token> rightVec;
    int i = low + 1;
    int end = vec.size();
    if (vec.at(vec.size() -1).data == "END") end = end -1;
    for (i; i < end; i++) {
        rightVec.push_back(vec[i]);
    }
    oper->rightChild = (build(rightVec));
    
    delete error;
    return oper;
}



// will cout the output in main
string stringAST(AST::Node* root, string equation) {
    //base case num or variable
    if (root->leftChild == nullptr && root->rightChild == nullptr) equation += root->data;
    if (root->type == "eq" || root->type == "op") {
        return "(" + stringAST(root->leftChild) + " " + root->data + " " + stringAST(root->rightChild) + ")";
    }

    return equation;

}

//
float evaluate(AST::Node* root){ 
    float result;
    // base case when data = number or variable
    if (root->leftChild == nullptr && root->rightChild == nullptr) {
        if (root->type == "var") {
            return 0;
        }
        return stof(root->data);}
    if (root->data == "=") {
        evaluate(root->rightChild);
    }

    else if (root->type == "op") {
        
        if (root->type == "+") {
            result = evaluate(root->leftChild) + evaluate(root->rightChild);
        }
        if (root->type == "-") {
            result = evaluate(root->leftChild) - evaluate(root->rightChild);
        }
        if (root->type == "*") {
            result = evaluate(root->leftChild) * evaluate(root->rightChild);
        }
        if (root->type == "/") {
            if (root->rightChild == 0) {
                cout << "\nRuntime error: division by zero."  << endl;
            }
            result = evaluate(root->leftChild) / evaluate(root->rightChild);
        }
        
    }

    return result;
}



int main() {
    vector<token> tokenVec;
    tokenVec = lexer();
    
    /*
    int i=0;
    for (i; i < tokenVec.size(); i++) {
        cout << tokenVec.at(i).data;
    }
    cout << endl << tokenVec.at(tokenVec.size()- 1).data;
    cout << endl << tokenVec.at(tokenVec.size()- 2).data; */
    AST tree;
    tree.root = build(tokenVec);


   /*cout << tree.root->data << " " << tree.root->type << endl;

    cout << tree.root->leftChild->data << " " << tree.root->leftChild->type << endl;
    cout << tree.root->rightChild->data << " " << tree.root->rightChild->type << endl;
*/
    string equation = stringAST(tree.root);
    cout << equation << endl;

    double result = evaluate(tree.root);
    cout << result; 

    
    return 0;
}