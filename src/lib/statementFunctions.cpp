#include "lex.h"
#include "scrypt.h"
#include "calc.h"

//FIX PRINT FUNCTION NOT ADDING ITS CHILDREN CORRECTLY IDK WHY?


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
    //cout << "in conversion" << endl;
    node2->data = node3->data;
    //cout << "fine with data" << endl;
    node2->type = node3->type;
    //cout << "fine with type" << endl;
    if (node3->children.size() != 0) {
    if (node3->children[0]) {
        //cout << "1" << endl;
        node2->leftChild = ConvertAST3ToAST2(std::move(node3->children[0]));
        //cout << "2" << endl;
    }

    if (node3->children[1]) {
        //cout << "3" << endl;
        node2->rightChild = ConvertAST3ToAST2(std::move(node3->children[1]));
        //cout << "4" << endl;
    }
    }


    return node2;
}

// also has to take in a vector of variables to be able to use if needed
unique_ptr<AST3::Node> buildProgram(vector<token> vec){
    // takes in vector and recursively creates an AST
    std::unique_ptr<AST3::Node> node = std::make_unique<AST3::Node>();
    int i = 0; 
    while (i < int(vec.size())) {
        //cout << "in loop" << endl;
        if (vec[i].type == "condition") {
            std::unique_ptr<AST3::Node> nodeChild = std::make_unique<AST3::Node>();
            nodeChild->type = "condition";
           //cout << "in condition!!" << endl;
            if (vec[i].data == "if" || vec[i].data == "while" || vec[i].data == "else if") {
                nodeChild->data = vec[i].data; 
                token parent = vec[i];
           //cout << "if or while or else if " << endl;

                // getting conditions 
                vector<token> expression;
                i++; 
                while(vec[i].data != "{") {
                    expression.push_back(vec[i]);
                    i++;
                }

                //for (int j= 0; j < int(expression.size()); j++) {
                //   cout << expression[j].data << endl;
                //}
               
                std::unique_ptr<AST2::Node> expressTree;
                try { 
                    expressTree = build(expression, parent);
                }
                catch (error Error){
                    if (Error.code == 2) {
                        throw(Error);
                    }
                };

                //cout << expressTree->data << endl;
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
                if (i > int(vec.size())-2) break;
                i++;
            }
            
            node->children.push_back(std::move(buildProgram(actions)));
        }

        // expressions 
        else if (vec[i].type == "var") {
            // grab the entire expression
            //cout << "in var!!!" << endl;
            
            int row = vec[i].row;
            vector<token> express;
            while (vec[i].row == row) {
                //cout << "in var while loop" << endl;
                express.push_back(vec[i]);
                i++; /// gets the while 
                if (i > int(vec.size()) -1 ) break;
                //cout << "past break" << endl;
                if (vec[i].type == "end") break;  
            }
            token emptyToken;
            //cout << "made it here" << endl;
            std::unique_ptr<AST2::Node> treeExpress;

            try { // build tree
                    treeExpress = build(express, emptyToken);
                }
                catch (error Error){
                    if (Error.code == 2) {
                        throw(Error);
                    }
                };
            //cout << treeExpress->data << endl;
            //cout << "past build" << endl;
            node->children.push_back(ConvertAST2ToAST3(std::move(treeExpress))); //converting AST2 to AST3
            //cout << "child: " << node->children[0]->data << endl;
            //cout << "past push back " << endl;
        }

        else if (vec[i].type == "print") {
            //cout << "building print" << endl;
            std::unique_ptr<AST3::Node> printNode = std::make_unique<AST3::Node>();
            printNode->data = "print";
            printNode->type = "print";

            // getting the expression that is being printed
            vector<token> output;
            
            int row = vec[i].row;
            i++;
            while(i < int(vec.size()) && vec[i].row == row) {
                //cout << "in while looooop" << endl;
                if (vec[i].data != ")" && vec[i].data != "(") {
                    output.push_back(vec[i]);
                    //cout << "vec[i].data" <<endl;
                }
                i++;
            }
            token emptyToken;
            printNode->children.push_back(ConvertAST2ToAST3(build(output, emptyToken)));
            node->children.push_back(std::move(printNode)); 
            
        }
    
        else i++;
        
    } 
    
    return node;
}; 


void runProgram(unique_ptr<AST3::Node> &root, vector<variable> &variables) {
    
    ifTrue boolCheck;  // checking to see if 

    for (int i=0; i < int(root->children.size()); i++) {
        // get type 
        if (root->data == "while") i++;
        string kidType = root->children[i]->type;
        string kidData = root->children[i]->data;
        //cout << "data: "<< kidData << endl;
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

                    if (kidData == "if") {
                        boolCheck.ft = true;
                        boolCheck.index = i; 
                    }

                    runProgram(root->children[i], variables);

                    if (kidData == "while") {
                    condition = evaluate(ast2, variables);
                    }

                    else condition.mBool = false;
                }
            }
            else {
                cout << "Runtime error: condition is not a bool." << endl;
                error Error;
                Error.code = 3;
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
            //cout << "in print!!!!!" << endl;
            std::unique_ptr<AST2::Node> out2 = ConvertAST3ToAST2(std::move(root->children[i]->children[0]));
            boolNum output;
            try{
            output = evaluate(out2, variables);
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
                throw(runtime);
            }
            if (output.mType == "bool") {
                if (output.mBool){ 
                    cout << "true" << endl;
                    }
                else {
                    cout << "false" << endl;
                }
            }
            else if (output.mType == "num") {
                cout << output.mNum << endl;
            }
            //cout << "end print" << endl;
    }
    }
}



