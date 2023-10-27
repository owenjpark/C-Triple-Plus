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
    // PRESCEDENCE AS FOLLOWS
    // "="      0
    // "+" "-"  1 
    // "*" "/"  2
    // "(" ")"  3
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


        else if(vec[i].data == "(" || vec[i].data == ")") {
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
    if (vec.at(opLeast).data == ")") {
        error rParenthError;
        rParenthError.data = vec.at(opLeast).data;
        rParenthError.row = vec.at(opLeast).row;
        rParenthError.column = vec.at(opLeast).column;
        rParenthError.code = 2;
        // cout << "throw2" << endl;
        throw rParenthError;
    }
    return opLeast;
}

unique_ptr<AST2::Node> build(vector<token> vec, token parentToken) {
    if (vec.size() == 1 || (vec.size() == 2 && vec.at(1).type == "end")) {
        if (vec.at(0).type == "num" || vec.at(0).type == "var") { // BASE CASE: vec has only num or variable (even if it includes END   )
            unique_ptr<AST2::Node> node(new AST2::Node);
            node->data = vec.at(0).data;
            node->row = vec.at(0).row;
            node->column = vec.at(0).column;
            node->type = vec.at(0).type;
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
        }
        else if (vec.at(0).type == "end") { // vec empty
            error empty;
            empty.code = 2;
            empty.column = vec.at(0).column;
            empty.data = "END";
            // cout << "throw1" << endl;
            throw(empty);
        }
        else if (vec.at(0).type == "lParenth") { // we don't know at this point size is 1
            error noFirstOperand;
            noFirstOperand.data = vec.at(1).data;
            noFirstOperand.row = vec.at(1).row;
            noFirstOperand.column = vec.at(1).column;
            noFirstOperand.code = 2;
            // cout << "throw2" << endl;
            throw noFirstOperand;
        }
        else {
            error noFirstOperand;
            noFirstOperand.data = vec.at(0).data;
            noFirstOperand.row = vec.at(0).row;
            noFirstOperand.column = vec.at(0).column;
            noFirstOperand.code = 2;
            // cout << "throw3" << endl;
            throw noFirstOperand;
        }
    }

    // case if argument is inside ()
    if (vec.at(0).data == "(") { // vec starts with "("
        unsigned i = 1; // go past parenthesis
        int paramCounter = 0;
        int parenthDiff = 1;

        while (parenthDiff != 0) {
            if (vec.at(i).type == "lParenth") {
                parenthDiff++;
            }
            else if (vec.at(i).type == "rParenth") {
                parenthDiff--;
            }
            if (i == vec.size() - 1 || parenthDiff == 0) { // break if i on last index
                break;
            }
            paramCounter++;
            i++;
        }
        // i should be at closing parenth or end of vector
        if (paramCounter < 1) {
            error emptyParenth;
            emptyParenth.data = vec.at(i).data;
            emptyParenth.row = vec.at(i).row;
            emptyParenth.column = vec.at(i).column;
            emptyParenth.code = 2;
            // cout << "throw4" << endl;
            throw emptyParenth;
        }
        if ((vec.size() - 1) > i) { // more indexes past i
            if (vec.at(i + 1).type == "end") {
                if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq") {
                    error parenthNumEnd;
                    parenthNumEnd.data = vec.at(i).data;
                    parenthNumEnd.row = vec.at(i).row;
                    parenthNumEnd.column = vec.at(i).column;
                    parenthNumEnd.code = 2;
                    // cout << "throw5.1" << endl;
                    throw parenthNumEnd;
                }
                vec.erase(vec.begin() + i); // NOTE: deleting end first
                vec.erase(vec.begin());
            }
        }
        else {
            if (vec.at(i).type != "rParenth") {
                error noClosingParenth;
                noClosingParenth.data = vec.at(i).data;
                noClosingParenth.row = vec.at(i).row;
                noClosingParenth.column = vec.at(i).column;
                noClosingParenth.code = 2;
                // cout << "throw5" << endl;
                throw noClosingParenth;
            }
            if (vec.at(i - 1).type == "op" || vec.at(i - 1).type == "eq") {
                error parenthNumEnd;
                parenthNumEnd.data = vec.at(i - 1).data;
                parenthNumEnd.row = vec.at(i - 1).row;
                parenthNumEnd.column = vec.at(i - 1).column;
                parenthNumEnd.code = 2;
                // cout << "throw5.2" << endl;
                throw parenthNumEnd;
            }
            vec.pop_back();
            vec.erase(vec.begin());
        }
    }

    int lowestPrecedenceI = precedence(vec);
    
    unique_ptr<AST2::Node> oper(new AST2::Node);
    oper->data = vec.at(lowestPrecedenceI).data;
    oper->row = vec.at(lowestPrecedenceI).row;
    oper->column = vec.at(lowestPrecedenceI).column;
    oper->type = vec.at(lowestPrecedenceI).type;

    vector<token> leftVec;
    for (int j = 0; j < lowestPrecedenceI; j++) {
        leftVec.push_back(vec[j]);
    }
    if (leftVec.size() == 0) {
        error invalidOp;
        invalidOp.data = vec.at(lowestPrecedenceI).data;
        invalidOp.row = vec.at(lowestPrecedenceI).row;
        invalidOp.column = vec.at(lowestPrecedenceI).column;
        invalidOp.code = 2;
        // cout << "throw6" << endl;
        throw invalidOp;
    }
    oper->leftChild = build(leftVec, vec.at(lowestPrecedenceI));
    
    vector<token> rightVec;
    for (unsigned i = lowestPrecedenceI + 1; i < vec.size(); i++) {
        rightVec.push_back(vec[i]);
    }
    if (rightVec.size() == 0) {
        error invalidOp;
        invalidOp.data = parentToken.data;
        invalidOp.row = parentToken.row;
        invalidOp.column = parentToken.column;
        invalidOp.code = 2;
        // cout << "throw7" << endl;
        throw invalidOp;
    }

    if(vec.at(lowestPrecedenceI).type == "eq") {
        if (leftVec.size() != 1) {
            error invalidEQ;
            invalidEQ.data = vec.at(lowestPrecedenceI).data;
            invalidEQ.row = vec.at(lowestPrecedenceI).row;
            invalidEQ.column = vec.at(lowestPrecedenceI).column;
            invalidEQ.code = 2;
            // cout << "throw7" << endl;
            throw invalidEQ;
        }
        if (leftVec.at(0).type != "var") {
            error invalidEQ;
            invalidEQ.data = vec.at(lowestPrecedenceI).data;
            invalidEQ.row = vec.at(lowestPrecedenceI).row;
            invalidEQ.column = vec.at(lowestPrecedenceI).column;
            invalidEQ.code = 2;
            // cout << "throw7" << endl;
            throw invalidEQ;
        }
    }

    oper->rightChild = build(rightVec, vec.at(lowestPrecedenceI));
    
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