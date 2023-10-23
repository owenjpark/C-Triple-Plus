#include "lib/calc.h"
#include "lib/lex.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

AST2::AST2() {
    root = nullptr;
};

AST2::~AST2() {
    clear(root);
}

void AST2::clear (Node* node){
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
    // grab the column and data for the operator with the leAST2 precedence
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
            while (vec[i].data != ")" || i < int(vec.size())) { 
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


AST2::Node* build(vector<token> vec) {
    // assumming there are no parse errors going in 
    // right now only the operators and parenthesis

    // call precedence and make that the node data 
    // store index of leAST2 
    // base case vec.size() = 1 or 0
    //AST2::Node* error = new AST2::Node(); 
    //error->data = "ERROR";
    string peak;
    
    if (vec.size() == 0) return nullptr;

    if (vec.at(vec.size() - 1).data == "END") {
        vec.pop_back();
        string peak = "END";
        }

    if (vec.size() == 0) return nullptr;

    AST2::Node* node = new AST2::Node;

    // base case: if the vector size 1 
    if (vec.size() == 1) {
        // can only be num or var
        if (vec.at(0).type == "num" || vec.at(0).type == "var") {
        node->data = vec.at(0).data;
        node->type = vec.at(0).type;
    }
    return node;
    }

    // checking valid parenthesis 
    // checking valid operation
    int count = 0;
    //int count = 0;
    bool nested = true;
    bool startParenth = false;
    if(vec.at(0).data == "(") startParenth = true;
    
    for (int j=0; j < int(vec.size()); j++) {
        if (vec.at(j).data == "(") {
            count += 1;
            if (j != 0) {
                string prev = vec.at(j-1).type;
                if (prev != "op" || prev != "rParenth") {
                    error left;
                    left.code = 2;
                    left.column = vec.at(j).column;
                    left.data = vec.at(j).data;
                    throw(left);
                }
            }
            }
        if (vec.at(j).data == ")") {
            count -= 1;
            if (j != int(vec.size()-1)) {
                string next = vec.at(j+1).type;
                if (next != "op" || next != "lParenth") {
                    error right;
                    right.code = 2;
                    right.column = vec.at(j+1).column;
                    right.data = vec.at(j+1).data;
                    throw(right);
                }
            }
            }
        if (count < 0) {
                error right1;
                right1.code = 2;
                right1.column = vec.at(j).column;
                right1.data = vec.at(j).data;
                throw(right1);
        }
        if (count == 0 && j != int(vec.size() - 1) ) nested = false;

        
        
    }
    if (count != 0) {
        error uneven;
        uneven.code = 2;
        uneven.column = vec.size() +1;
        uneven.data = "END";
        delete node;
        throw(uneven);
    }
    
    if (nested && startParenth) {
    vec.erase(vec.begin());
    vec.pop_back();

    peak = ")";
    }
            
    
    
    int low = 0; // index of lowest precedence operation
    //AST2::Node* oper = new AST2::Node();

    low = precedence(vec);
    node->data = vec.at(low).data;
    node->type = vec.at(low).type;
    if (node->type == "num" || node->type == "var") {
        error numVar;
        numVar.code = 2;
        numVar.column = vec.at(1).column;
        numVar.data = node->data; //might have to change if there is more than 2 
        throw(numVar);
    }

    // then call precedence of right side vec[leAST2 + 1] vec.size() - 1  <-- right child points to result 
    vector<token> rightVec;

    int end = vec.size();
    for (int i = low +1; i < end; i++) {
        rightVec.push_back(vec[i]);
    }

    if (rightVec.size() == 0) {
        error noright;
        noright.column = vec.at(low).column +1;
        noright.code = 2; 
        noright.data = peak;
        
        throw(noright);
    }
    node->rightChild = (build(rightVec));


     //call build with left side vec[0] - vec[leAST2-1]        <-- leftchild points to result
    
    vector<token> leftVec;
    
    for (int j = 0; j < low; j++) {
        leftVec.push_back(vec[j]);
    }
    if (leftVec.size() == 0) {
        error noleft;
        noleft.column = low;
        noleft.code = 2; 
        noleft.data = node->data;
        throw(noleft);
    }

    node->leftChild = (build(leftVec));
    
    return node;
}



// will cout the output in main
string stringAST2(AST2::Node* root, string equation) {
    //base case num or variable
    if (root->leftChild == nullptr && root->rightChild == nullptr) {
        if (root->type == "ERROR") return "ERROR";
        equation += root->data;
        }
    if (root->type == "eq" || root->type == "op") {
        //change for equal
        return "(" + stringAST2(root->leftChild) + " " + root->data + " " + stringAST2(root->rightChild) + ")";
    }

    return equation;

}

//
float evaluate(AST2::Node* root, float result){ 
    // base case when data = number or variable
    if (root->leftChild == nullptr && root->rightChild == nullptr) {
        if (root->type == "var") {
            // change to be like number
            //unless unidentified not in the vector
            return 0;
        }
    
        // if its a num
        result = stof(root->data);
        }

    if (root->data == "=") {
        result =  evaluate(root->rightChild);
        // store result with variable name in vector
    }

    else if (root->type == "op") {
        if (root->data == "+") {
            result = evaluate(root->leftChild) + evaluate(root->rightChild);
        }
        if (root->data == "-") {
            result = evaluate(root->leftChild) - evaluate(root->rightChild);
        }
        if (root->data == "*") {
            result = evaluate(root->leftChild) * evaluate(root->rightChild);
        }
        if (root->data == "/") {
            float right = evaluate(root->rightChild);
            if (right == 0) {
                cout << "\nRuntime error: division by zero."  << endl;
            }
            result =  evaluate(root->leftChild) / right;
        }
        
    }
return result;
}


int main() {
    string line; 
    while(std::getline(std::cin, line)) {
    vector<token> tokenVec;
    createTokens(line, 1, tokenVec);
    /*
    int i=0;
    for (i; i < tokenVec.size(); i++) {
        cout << tokenVec.at(i).data;
    }
    cout << endl << tokenVec.at(tokenVec.size()- 1).data;
    cout << endl << tokenVec.at(tokenVec.size()- 2).data; */
    AST2 tree;
    try{
        tree.root = build(tokenVec);
    }
    catch(error Error){
         if ( Error.code == 2) {
             cout << "Unexpected token at line 1 column " << Error.column << ": " << Error.data << endl;
    
        tree.clear(tree.root);
        }
        continue;
        
    }
    
    tree.root = build(tokenVec);


   /*cout << tree.root->data << " " << tree.root->type << endl;

    cout << tree.root->leftChild->data << " " << tree.root->leftChild->type << endl;
    cout << tree.root->rightChild->data << " " << tree.root->rightChild->type << endl;
*/
    string equation = stringAST2(tree.root);
    cout << equation << endl;

    double result = evaluate(tree.root);
    cout << result << endl;
    }
    
    return 0;
}