#include "lex.h"
#include "scrypt.h"
#include "calc.h"

// converting AST2 to AST3 to store in the same tree
unique_ptr<AST3::Node> ConvertAST2ToAST3(unique_ptr<AST2::Node> &node2) {
    unique_ptr<AST3::Node> node3 = make_unique<AST3::Node>();
    node3->data = node2->data;
    node3->type = node2->type;

    if (node2->leftChild) {
        node3->children.push_back(ConvertAST2ToAST3(node2->leftChild));
    }

    if (node2->rightChild) {
        node3->children.push_back(ConvertAST2ToAST3(node2->rightChild));
    }
    return node3;
}

//converting AST3 to AST2
unique_ptr<AST2::Node> ConvertAST3ToAST2(unique_ptr<AST3::Node> &node3) {
    unique_ptr<AST2::Node> node2 = make_unique<AST2::Node>();
    node2->data = node3->data;
    node2->type = node3->type;
    if (node3->children.size() != 0) {
        if (node3->children.at(0)) {
            node2->leftChild = ConvertAST3ToAST2(node3->children.at(0));
        }

        if (node3->children.at(1)) {
            node2->rightChild = ConvertAST3ToAST2(node3->children.at(1));
        }
    }
    return node2;
}

unique_ptr<AST3::Node> buildProgram(vector<token> vec){ // takes in vector and recursively creates an AST
    unique_ptr<AST3::Node> node = make_unique<AST3::Node>();

    int i = 0;
    while (i < int(vec.size())) {
        if (vec.at(i).type == "condition") {
            unique_ptr<AST3::Node> nodeChild = make_unique<AST3::Node>();
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "condition";

            if (vec.at(i).data == "if" || vec.at(i).data == "while") {
                // getting conditions
                vector<token> expression;
                i++; 
                while(vec.at(i).data != "{") {
                    expression.push_back(vec.at(i));
                    i++;
                }
                unique_ptr<AST2::Node> expressTree;
                token emptyToken;
                expressTree = build(expression, emptyToken);
                nodeChild->children.push_back(ConvertAST2ToAST3(expressTree)); // converting AST2 to AST3

                // at "{"
                i++;
                // at first token within block
                vector<token> blockVec;
                int brackDiff = 1;
                while (brackDiff != 0) {
                    if (vec.at(i).data == "{") {
                        brackDiff++;
                    }
                    else if (vec.at(i).data == "}") {
                        brackDiff--;
                    }
                    if (brackDiff == 0) {
                        break;
                    }
                    blockVec.push_back(vec.at(i));
                    i++;
                }
                // i at }
                for (unsigned j = 0; j < buildProgram(blockVec)->children.size(); j++) {
                    nodeChild->children.push_back(move(buildProgram(blockVec)->children.at(j)));
                }
                node->children.push_back(move(nodeChild));
            }
            else if (vec.at(i).data == "else") {
                if (vec.at(i + 1).data == "if") {
                    i++;

                    unique_ptr<AST3::Node> nodeChildChild = make_unique<AST3::Node>();
                    nodeChildChild->data = vec.at(i).data;
                    nodeChildChild->type = "condition";

                    i++;
                    // at express

                    vector<token> expression;
                    while(vec.at(i).data != "{") {
                        expression.push_back(vec.at(i));
                        i++;
                    }
                    // at "{"
                    unique_ptr<AST2::Node> expressTree;
                    token emptyToken;
                    expressTree = build(expression, emptyToken);
                    
                    nodeChildChild->children.push_back(ConvertAST2ToAST3(expressTree));
                    
                    i++;
                    // at first token within block
                    vector<token> blockVec;
                    int brackDiff = 1;
                    while (brackDiff != 0) {
                        if (vec.at(i).data == "{") {
                            brackDiff++;
                        }
                        else if (vec.at(i).data == "}") {
                            brackDiff--;
                        }
                        if (brackDiff == 0) {
                            break;
                        }
                        blockVec.push_back(vec.at(i));
                        i++;
                    }
                    // i at }
                    unique_ptr<AST3::Node> block(new AST3::Node);
                    block = buildProgram(blockVec);
                    for (unsigned j = 0; j < block->children.size(); j++) {
                        nodeChildChild->children.push_back(move(block->children.at(j))); // out of range
                    }
                    nodeChild->children.push_back(move(nodeChildChild));
                    node->children.push_back(move(nodeChild));
                    continue;
                }
                while(vec.at(i).data != "{") {
                    i++;
                }
                i++;
                // at first token within block
                vector<token> blockVec;
                int brackDiff = 1;
                while (brackDiff != 0) {
                    if (vec.at(i).data == "{") {
                        brackDiff++;
                    }
                    else if (vec.at(i).data == "}") {
                        brackDiff--;
                    }
                    if (brackDiff == 0) {
                        break;
                    }
                    blockVec.push_back(vec.at(i));
                    i++;
                }
                // i at }
                for (unsigned j = 0; j < buildProgram(blockVec)->children.size(); j++) {
                    nodeChild->children.push_back(move(buildProgram(blockVec)->children.at(j)));
                }
                node->children.push_back(move(nodeChild));
            }            
        }
        else if (vec.at(i).type == "var") { // TODO: what if variable starts with "(" e.g. (x = 12)
            int row = vec.at(i).row; // set row to current row
            vector<token> express;
            while (vec.at(i).row == row) {
                express.push_back(vec.at(i));
                i++;
                if (i > int(vec.size()) - 1) {
                    break;
                }
                if (vec.at(i).type == "end") {
                    break;
                }
            }
            unique_ptr<AST2::Node> treeExpress;
            token emptyToken;

            treeExpress = build(express, emptyToken);
            node->children.push_back(ConvertAST2ToAST3(treeExpress)); //converting AST2 to AST3
        }
        else if (vec.at(i).type == "print") {
            unique_ptr<AST3::Node> printNode = make_unique<AST3::Node>();
            printNode->data = "print";
            printNode->type = "print";

            // getting the expression that is being printed
            vector<token> output;
            
            int row = vec.at(i).row;
            i++;
            while(i < int(vec.size()) && vec.at(i).row == row) {
                output.push_back(vec.at(i));
                i++;
            }
            token emptyToken;
            AST2 tree;
            tree.root = build(output, emptyToken);
            printNode->children.push_back(ConvertAST2ToAST3(tree.root));
            node->children.push_back(move(printNode)); 
        }
        else {
            i++;
        }
    }  
    return node;
}

bool enterStatement (unique_ptr<AST3::Node> &root, vector<variable> &variables) { // helper function for runProgram
    unique_ptr<AST2::Node> ast2 = ConvertAST3ToAST2(root->children.at(0));
    boolNum condition;
    try {
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
        if (root->data == "if") {
            if (condition.mBool == true) { // continue to run program
                return true;
            }
        }
    }
    return false;
}

void runProgram(unique_ptr<AST3::Node> &root, vector<variable> &variables) {
    unsigned i = 0;
    if (root->data == "if" || root->data == "else if" || root->data == "while") {
        unique_ptr<AST2::Node> ast2 = ConvertAST3ToAST2(root->children.at(0));
        boolNum condition;
        try {
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
            if (root->data == "if") {
                if (condition.mBool == true) { // continue to run program
                    i++;
                }
                else { // stop running program
                    return;
                }
            }
            else if (root->data == "while") {
                if (condition.mBool == true) { // continue to run program
                    i++;
                }
                else { // stop running program
                    return;
                }
            }
            else if (root->data == "else if") {
                i++;
            }
            else if (root->data == "else") {
                i++;
            }
            
            // TODO: implement rest of conditionals
        }
        else {
            cout << "Runtime error: condition is not a bool." << endl;
            error Error;
            Error.code = 3;
            throw(Error);
        }
    }
    bool entered = false;
    for (; i < root->children.size(); i++) {
        string kidType = root->children.at(i)->type; // seg fault
        string kidData = root->children.at(i)->data;
        if (kidType == "op" || kidType == "eq" || kidType == "eqIneq" || kidType == "logicOp") {
            entered = false;
            // convert AST3 into AST2 
            unique_ptr<AST2::Node> ast2root = ConvertAST3ToAST2(root->children.at(i));
            // call evaluate function and save result into variables 
            boolNum result;
            try {
                result = evaluate(ast2root, variables);
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
            }
        }
        else if (kidData == "print") {
            entered = false;
            unique_ptr<AST2::Node> out2 = ConvertAST3ToAST2(root->children.at(i)->children.at(0));
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
        else if (kidData == "if") {
            if (enterStatement(root->children.at(i), variables) == true) {
                entered = true;
            }
            runProgram(root->children.at(i), variables);
        }
        else if (kidData == "while") {
            runProgram(root->children.at(i), variables);
        }
        else if (kidData == "else") {
            if (entered != true) {
                runProgram(root->children.at(i), variables);
            }
        }
    }
    if (root->data == "while") {
        runProgram(root, variables);
    }
}



