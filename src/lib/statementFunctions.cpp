#include "lex.h"
#include "scrypt.h"
#include "calc.h"

// converting AST2 to AST3 to store in the same tree
std::unique_ptr<AST3::Node> ConvertAST2ToAST3(std::unique_ptr<AST2::Node> node2) {
    unique_ptr<AST3::Node> node3 = std::make_unique<AST3::Node>();
    node3->data = node2->data;
    node3->type = node2->type;

    if (node2->leftChild) {
        node3->children.push_back(ConvertAST2ToAST3(std::move(node2->leftChild)));
    }

    if (node2->rightChild) {
        node3->children.push_back(ConvertAST2ToAST3(std::move(node2->rightChild)));
    }

    return node3;
}

//converting AST3 to AST2
std::unique_ptr<AST2::Node> ConvertAST3ToAST2(std::unique_ptr<AST3::Node> node3) {
    unique_ptr<AST2::Node> node2 = std::make_unique<AST2::Node>();
    node2->data = node3->data;
    node2->type = node3->type;
    if (node3->children.size() != 0) {
        if (node3->children[0]) {
            node2->leftChild = ConvertAST3ToAST2(std::move(node3->children[0]));
        }

        if (node3->children[1]) {
            node2->rightChild = ConvertAST3ToAST2(std::move(node3->children[1]));
        }
    }

    return node2;
}

unique_ptr<AST3::Node> buildProgram(vector<token> vec){ // takes in vector and recursively creates an AST
    std::unique_ptr<AST3::Node> node = std::make_unique<AST3::Node>();
    int i = 0; 
    while (i < int(vec.size())) {
        if (vec[i].type == "condition") {
            std::unique_ptr<AST3::Node> nodeChild = std::make_unique<AST3::Node>();
            nodeChild->type = "condition";

            if (vec[i].data == "if" || vec[i].data == "while" || vec[i].data == "else if") {
                nodeChild->data = vec[i].data; 
                token parent = vec[i];
                // getting conditions 
                vector<token> expression;
                i++; 
                while(vec[i].data != "{") {
                    expression.push_back(vec[i]);
                    i++;
                }
               
                std::unique_ptr<AST2::Node> expressTree;
                try { 
                    expressTree = build(expression, parent);
                }
                catch (error Error){
                    if (Error.code == 2) {
                        cout << "test1" << endl;
                        throw(Error);
                    }
                };

                nodeChild->children.push_back(ConvertAST2ToAST3(std::move(expressTree))); //converting AST2 to AST3
                // call build on vec i+1 until {
                // save vec into children 
                // call buildprogram on { to }
                node->children.push_back(std::move(nodeChild));
            }
            
            else if (vec[i].data == "else") {
                while(vec[i].data != "{") {
                    i++;
                }
                i++; // starting at the token after {
            }
            // getting what is inside of {}
            vector<token> actions;

            while(vec[i].data != "}" ) {
                actions.push_back(vec[i]);
                i++;
            }
            
            node->children.push_back(buildProgram(actions));
        }
        else if (vec[i].type == "var") {
            // grab the entire expression
            int row = vec[i].row; // set row to current row
            vector<token> express;
            while (vec[i].row == row) {
                express.push_back(vec[i]);
                i++;
                if (i > int(vec.size()) - 1) {
                    break;
                }
                if (vec[i].type == "end") {
                    break;
                }
            }

            token emptyToken;
            std::unique_ptr<AST2::Node> treeExpress;
            try { // build tree
                    treeExpress = build(express, emptyToken);
            }
            catch (error Error){
                if (Error.code == 2) {
                    cout << "test2" << endl;
                    throw(Error);
                }
            }
            node->children.push_back(ConvertAST2ToAST3(std::move(treeExpress))); //converting AST2 to AST3
        }
        else if (vec[i].type == "print") {
            std::unique_ptr<AST3::Node> printNode = std::make_unique<AST3::Node>();
            printNode->data = "print";
            printNode->type = "print";

            // getting the expression that is being printed
            vector<token> output;
            
            int row = vec[i].row;
            i++;
            while(i < int(vec.size()) && vec[i].row == row) {
                if (vec[i].data != ")" && vec[i].data != "(") {
                    output.push_back(vec[i]);
                }
                i++;
            }
            token emptyToken;
            printNode->children.push_back(ConvertAST2ToAST3(build(output, emptyToken)));
            node->children.push_back(std::move(printNode)); 
        }
        else {
            i++;
        }
    } 
    
    return node;
}


void runProgram(unique_ptr<AST3::Node> &root, vector<variable> &variables) {
    // run through children and evaluate 
    // store variables in vector to be used for later 
    // convert AST3 to AST2 for evaluating expressions 
    // make sure there is an if before an else in children vector 
    ifTrue boolCheck; 
    for (int i=0; i < int(root->children.size()); i++) {
        // get type 
        if (root->data == "while") i++;
        string kidType = root->children[i]->type;
        string kidData = root->children[i]->data;
        if (kidType == "op" || kidType == "eq" || kidType == "eqIneq" || kidType == "logicOp"){
            // convert AST3 into AST2 
            //cout << "inside runProgram op/eq/eqIneq/logicOp" << endl;
            std::unique_ptr<AST2::Node> ast2root = ConvertAST3ToAST2(std::move(root->children[i]));
            //cout << "did the AST3 to AST2 conversion" << endl;
            // call evaluate function and save result into variables 
            boolNum result;
            try{
                result = evaluate(ast2root, variables);
               // cout << "evaluated" << endl;
            }
            catch(error runtime){
                if (runtime.code == 0) {
                    cout << "Runtime error: division by zero."  << endl;
                }
                else if (runtime.code == 3) {
                    cout << "Runtime error: unknown identifier " << runtime.data << endl;
                }
                else if (runtime.code == 4) {
                    cout << "Runtime error: invalid operand type." << endl;
                }
            };
        }
        else if (kidType == "condition") {
            if (kidData == "if" || kidData == "else if" || kidData == "while") {
                // evaluate first child and make sure it is a bool 
                boolNum condition;
                std::unique_ptr<AST2::Node> ast2 = ConvertAST3ToAST2(std::move(root->children[i]->children[0]));

                try{
                condition = evaluate(ast2, variables);
                }
                catch(error runtime){
                    if (runtime.code == 0) {
                        cout << "Runtime error: division by zero."  << endl;
                    }
                    else if (runtime.code == 3) {
                        cout << "Runtime error: unknown identifier " << runtime.data << endl;
                    }
                    else if (runtime.code == 4) {
                        cout << "Runtime error: invalid operand type." << endl;
                    }
                    runtime.code = 3;
                    cout << "test3" << endl;
                    throw(runtime);
                }
            if (condition.mType == "bool") {
                // if it is a bool evaluate if, else if, and while if its true
                // only while repeats, other 2 break after first loop
                while (condition.mBool) {
                    // evaluate all children 
                    if (kidData == "else if") {
                        if (boolCheck.ft == false && boolCheck.index == i - 1);
                        else break;
                    }
                    runProgram(root->children[i], variables);
                    if (kidData == "if") {
                        boolCheck.ft = true;
                        boolCheck.index = i; 
                    }
                    if (kidData == "while") condition = evaluate(ast2, variables);
                    else condition.mBool = false;
                }
            }
            else {
                cout << "Runtime error: condition is not a bool." << endl;
                error Error;
                Error.code = 3;
                cout << "test4" << endl;
                throw(Error);
            }
            }
            
            else if (kidData == "else") {
                if (boolCheck.ft == false) {
                runProgram(root->children[i], variables);
                }
            }
        }
        else if (kidData == "print") {
            std::unique_ptr<AST2::Node> out2 = ConvertAST3ToAST2(std::move(root->children[i]->children[0]));
            boolNum output;
            try {
                output = evaluate(out2, variables);
            }
            catch(error runtime) {
                if (runtime.code == 0) {
                    cout << "Runtime error: division by zero."  << endl;
                }
                else if (runtime.code == 3) {
                    cout << "Runtime error: unknown identifier " << runtime.data << endl;
                }
                else if (runtime.code == 4) {
                    cout << "Runtime error: invalid operand type." << endl;
                }
                runtime.code = 3;
                cout << "test5" << endl;
                throw(runtime);
            }

            if (output.mType == "bool") {
                if (output.mBool) {
                    cout << "true" << endl;
                }
                else {
                    cout << "false" << endl;
                }
            }
            else if (output.mType == "num") {
                cout << output.mNum << endl;
            }
        }
    }
}



