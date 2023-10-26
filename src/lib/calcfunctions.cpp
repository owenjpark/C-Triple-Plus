#include "calc.h"
#include "lex.h"

AST2::AST2() {
    root = nullptr;
};

AST2::~AST2() {
}

int findMatchingParenth(int i, vector<token> tokenVec) { // (12 + 7) should start at 12, returns index of )
    int parenthDiff = 1;
    while (parenthDiff != 0 && !(tokenVec.at(i).type == "end")) {
        if (tokenVec.at(i).type == "lParenth") {
            parenthDiff++;
        }
        if (tokenVec.at(i).type == "rParenth") {
            parenthDiff--;
        }
        if (parenthDiff == 0) { // so it doesn't over iterate
            break;
        }
        i++;
    }
    if (tokenVec.at(i).type == "end") {
        error someError(tokenVec.at(i).data, 1, tokenVec.at(i).column, 2);
        throw someError;
    }

    return i;
}

void expressionChecker2(unsigned startIndex, unsigned endIndex, bool inNested, vector<token> tokenVec) {
    if (tokenVec.size() == 1) { // only end token
        error someError(tokenVec.at(0).data, 1, tokenVec.at(0).column, 2);
        throw someError;
    }
    // has to have at least 1 real token in it
    if (tokenVec.at(startIndex).type != "num" && tokenVec.at(startIndex).type != "var" && tokenVec.at(startIndex).type != "lParenth") { // doesn't start with big 3
        error someError(tokenVec.at(startIndex).data, 1, tokenVec.at(startIndex).column, 2);
        throw someError;
    }
    // at least 1 element
    for (unsigned i = startIndex; i < endIndex; i++) { // doesn't hit end token
        if (tokenVec.at(i).type == "lParenth") {
            i++; // skip "("
            int endParenthIndex = findMatchingParenth(i, tokenVec);
            expressionChecker2(i, endParenthIndex, true, tokenVec);
            i = endParenthIndex;
            
            if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "end" && tokenVec.at(i + 1).type != "rParenth") { // after lParenth
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "num" || tokenVec.at(i).type == "var") {
            if (!inNested) { // edge case: only allow ")" in nested expression
                if (tokenVec.at(i + 1).type == "rParenth") {
                    error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                    throw someError;
                }
            }
            if (tokenVec.at(i + 1).type != "op" && tokenVec.at(i + 1).type != "eq" && tokenVec.at(i + 1).type != "end" && tokenVec.at(i + 1).type != "rParenth") {
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "op") {
            if (tokenVec.at(i + 1).type != "num" && tokenVec.at(i + 1).type != "var" && tokenVec.at(i + 1).type != "lParenth") {
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
        if (tokenVec.at(i).type == "eq") {
            if (tokenVec.at(i - 1).type != "var") {
                error someError(tokenVec.at(i).data, 1, tokenVec.at(i).column, 2);
                throw someError;
            }
            if (tokenVec.at(i + 1).type != "num" && tokenVec.at(i + 1).type != "var" && tokenVec.at(i + 1).type != "lParenth") {
                error someError(tokenVec.at(i + 1).data, 1, tokenVec.at(i + 1).column, 2);
                throw someError;
            }
        }
    }
}


// helper function for build 
//should be working but have to add symbols and equal sign 
int precedence(vector<token> vec) {
   
    //  1 for + -, 2 for * /, 3 for () and numbers
    // grab the column and data for the operator with the least precedence


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

unique_ptr<AST2::Node> build(vector<token> vec) {
    int length = vec.size();
    if (length == 1 || (length == 2 && vec.at(1).type == "end")) {
        if (vec.at(0).type == "num" || vec.at(0).type == "var") { // BASE CASE: vec has only num or variable 
            unique_ptr<AST2::Node> node(new AST2::Node);
            node->data = vec.at(0).data;
            node->type = vec.at(0).type;
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
        }
        if (vec.at(0).type == "end") { // vec empty
            error empty;
            empty.code = 2;
            empty.column = vec.at(0).column;
            empty.data = "END";
            throw(empty);
        }
    }

    if (vec.at(0).data == "(") { // vec starts with "("
        int count = 0;
        bool nested = true;
       
        for (int j= 0; j < length; j++) { // iterate through vector
            if (vec.at(j).data == "(") count += 1;
            if (vec.at(j).data == ")") count -= 1;
            if (count < 0) {
                 error tooRight;
                 tooRight.code = 2;
                 tooRight.column = vec.at(j).column;
                 tooRight.data = ")";
            }
            if (count == 0 && vec.at(j).data == ")") {
                if (vec.at(length - 1).data == "END") { // if end token still on vector
                    if (j != length - 2) nested = false; // if last index (not including end token), nested is false
                }
                else if (j != length - 1) nested = false; // if not last index, nested is false
            }
        }

        if (count != 0) { // mismatching parenths (maybe delete: seg fault?)
            error uneven;
            uneven.data = "END";
            uneven.code = 2;
            uneven.column = vec.at(length -1).column;
            throw(uneven);
        }
        
        if (nested) {
            if (vec.at(length - 1).data == "END") { // deleting parenthesis
                length = length - 1;
                vec.erase(vec.begin() + vec.size() - 2);
            }

            vec.erase(vec.begin()); // deleting "("
            vec.pop_back(); // deleting ")"
            length = length - 2;
        }
    }

    int low = 0; // index of lowest precedence operation
    unique_ptr<AST2::Node> oper(new AST2::Node);
    low = precedence(vec);
    
    oper->data = vec.at(low).data;
    oper->type = vec.at(low).type;

    // then call build with left side vec[0] - vec[least - 1]        <-- leftchild points to result
    if (int(vec.size()) > 1) {
        vector<token> leftVec;
        for (int j = 0; j < low; j++) {
            leftVec.push_back(vec[j]);
        }
        // if (leftVec.size() == 0) {
        //     error someError;
        //     someError.data = vec.at(low).data;
        //     someError.code = 2;
        //     someError.column = vec.at(low).column;
        //     throw(someError);
        // }
        oper->leftChild = (build(leftVec));
        
        // TODO: errors here

        // then call precedence of right side vec[least + 1] vec.size() - 1  <-- right child points to result 
        vector<token> rightVec;

        int end = vec.size(); 
        for (int i = low + 1; i < end; i++) {
            rightVec.push_back(vec[i]);
        }
        // if (rightVec.size() == 0) {
        //     error someError;
        //     someError.data = vec.at(low + 1).data;
        //     someError.code = 2;
        //     someError.column = vec.at(low + 1).column;
        //     throw(someError);
        // }
        oper->rightChild = (build(rightVec));
    }
    return oper;
}



// will cout the output in main
string stringAST2(unique_ptr<AST2::Node> &root, string equation) {
    //base case num or variable
    if (root->leftChild == nullptr && root->rightChild == nullptr) equation += root->data;
    if (root->type == "op" || root->type == "eq") {
        return "(" + stringAST2(root->leftChild) + " " + root->data + " " + stringAST2(root->rightChild) + ")";
    }

    return equation;

}

//
float evaluate(unique_ptr<AST2::Node> &root, vector<variable> &variables, float result){ 
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
            if (!update) variables.push_back(var);
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