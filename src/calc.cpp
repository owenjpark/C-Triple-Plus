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
   int temp = 6;


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
            while (vec[i].data != ")" && i < int(vec.size())) {
                i++;
            }
        }
        // if its a number or variable
        else {
            temp = 4;
        }

        if (temp <= rating) {
            // for assignment 
            if (rating == 0 && temp == 0);
            else {
            rating = temp;
            opLeast = i;
            }
        }

        i++;
    }
    return opLeast;
}



AST2::Node* build(vector<token> vec) {
    int length = vec.size();
    if (length == 1 || (length == 2 && vec.at(1).type == "end")) {
        //if size = 1;
        if (vec.at(0).type == "num" || vec.at(0).type == "var") {
        
        AST2::Node* node = new AST2::Node();
        node->data = vec.at(0).data;
        node->type = vec.at(0).type;
        node->leftChild = nullptr;
        node->rightChild = nullptr;
        return node;
        }


        if (vec.at(0).type == "end") {
            error empty;
            empty.code = 2;
            empty.column = vec.at(0).column;
            empty.data = "END";
            throw(empty);
        }
   
    
    }

    // case if argument is inside ()
    if (vec.at(0).data == "(") {
        int count = 0;
        bool nested = true;
       
        for (int j=0; j < length; j++) {
            if (vec.at(j).data == "(") count += 1;
            if (vec.at(j).data == ")") count -= 1;
            if (count < 0) {
                 error tooRight;
                 tooRight.code = 2;
                 tooRight.column = vec.at(j).column;
                 tooRight.data = ")";
            }
            if (count == 0 && vec.at(j).data == ")") {
                if (vec.at(length - 1).data == "END") {
                    if (j != length -2) nested = false;
                }
                else if ( j != length - 1) nested = false;
            }
            
        }
    
        if (count != 0) {
            error uneven;
            uneven.data = "END";
            uneven.code = 2;
            uneven.column = vec.at(length -1).column;
            throw(uneven);
        }
        
        if (nested) {
            if (vec.at(length - 1).data == "END") {
                length = length - 1;
                vec.pop_back();
            }

            vec.erase(vec.begin());
            vec.pop_back();
            length = length - 2;
        }
    }

 
    if (vec.at(length - 1).data == "END") {
        vec.pop_back();}

    int low = 0; // index of lowest precedence operation
    AST2::Node* oper = new AST2::Node();
    low = precedence(vec);
    
    oper->data = vec.at(low).data;
    oper->type = vec.at(low).type;

    // then call build with left side vec[0] - vec[leAST2-1]        <-- leftchild points to result
    if (int(vec.size()) > 1) {

    vector<token> leftVec;
    for (int j = 0; j < low; j++) {
        leftVec.push_back(vec[j]);
    }
    oper->leftChild = (build(leftVec));
   
    // then call precedence of right side vec[leAST2 + 1] vec.size() - 1  <-- right child points to result 
    vector<token> rightVec;

    int end = vec.size(); 
    for (int i = low +1; i < end; i++) {
        rightVec.push_back(vec[i]);
    }
    oper->rightChild = (build(rightVec));

    
    }
    return oper;
}



// will cout the output in main
string stringAST2(AST2::Node* root, string equation) {
    //base case num or variable
    if (root->leftChild == nullptr && root->rightChild == nullptr) equation += root->data;
    if (root->type == "op" || root->type == "eq") {
        return "(" + stringAST2(root->leftChild) + " " + root->data + " " + stringAST2(root->rightChild) + ")";
    }

    return equation;

}

//
float evaluate(AST2::Node* root, vector<variable> &variables, float result){ 
    // base case when data = number or variable
    if (root->leftChild == nullptr && root->rightChild == nullptr) {
        if (root->type == "var") {
            bool assigned = false;
            if (variables.size() > 0) {
            for (int i = 0; i < int(variables.size()); i++){
                
                if (variables[i].name == root->data) {
                    assigned = true;
                    return variables[i].value;
                } 
            } 
            }
            if (!assigned) {
                error unassigned;
                unassigned.code = 3;
                unassigned.data = root->data;
                throw(unassigned); 
            }
        }
        else {
        result = stof(root->data);
        return result;
        }
        }

    if (root->data == "=") {
        result =  evaluate(root->rightChild, variables);
        variable var;
        var.name = root->leftChild->data;
        var.value = result; 
        if (variables.size() == 0 ) variables.push_back(var);
        else{
            bool update = false;
        for (int i = 0; i < int(variables.size()); i++) {
            if (variables[i].name == var.name) {
                variables[i].value = result;
                update = true;
            }
            
        }
        if (update == false) variables.push_back(var);
        return result;
        }
    }
       
    

    else if (root->type == "op") {
        if (root->data == "+") {
            result = evaluate(root->leftChild, variables) + evaluate(root->rightChild, variables);
        }
        if (root->data == "-") {
            result = evaluate(root->leftChild, variables) - evaluate(root->rightChild, variables);
        }
        if (root->data == "*") {
            result = evaluate(root->leftChild, variables) * evaluate(root->rightChild, variables);
        }
        if (root->data == "/") {
            float right = evaluate(root->rightChild, variables);
            if (right == 0) {
                error zero;
                zero.code = 0;
                throw(zero);
            }
            result =  evaluate(root->leftChild, variables) / right;
        }
    }
return result;
}



int main() {
    string line;
    vector<variable> variables;

    while(std::getline(std::cin, line)) {
    vector<token> testVec;
    vector<token> tokenVec;
    try{
    createTokens(line, 1, tokenVec);
    }
    catch(int exitCode){
        continue;
    }

    if (tokenVec.size() != 0) {
    int col = tokenVec.at(tokenVec.size()-1).column;
    addEndToken(tokenVec, 1, col);
    }

    else {
        token end;
        end.column = 1;
        end.data = "END";
        end.type = "end";
        end.row = 1;
        tokenVec.push_back(end);
    }


    //tokenVec = lexer();
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
         }
        tree.clear(tree.root);
        continue;
       
    }
   

    

   /*cout << tree.root->data << " " << tree.root->type << endl;


    cout << tree.root->leftChild->data << " " << tree.root->leftChild->type << endl;
    cout << tree.root->rightChild->data << " " << tree.root->rightChild->type << endl;
*/
    string equation = stringAST2(tree.root);
    cout << equation << endl;

    try{
       double result =  evaluate(tree.root, variables, 0);
        cout << result << endl;
    }
    catch(error Runtime){
        if (Runtime.code == 3) {
            cout << "Runtime error: unknown identifier " << Runtime.data << endl;
        }
        else if (Runtime.code == 0) {
            cout << "Runtime error: division by zero."  << endl;
        }
        continue;
    }


    }
   
    return 0;
}



