
#include "lex.h"
#include "scrypt.h"
#include "calc.h"

unique_ptr<AST3::Node> ConvertAST2ToAST3(unique_ptr<AST2::Node> &node2) { // helper function; converts AST2 to AST3; returns root node
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

unique_ptr<AST2::Node> ConvertAST3ToAST2(unique_ptr<AST3::Node> &node3) { // helper function; converts AST3 to AST2; returns root node
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

vector<token> parseBlock(unsigned &i, const vector<token> &vec) { // helper function for build program; creates a vector of all tokens within block
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
    return blockVec;
}

unique_ptr<AST3::Node> buildProgram(const vector<token> &vec) {
    unique_ptr<AST3::Node> node = make_unique<AST3::Node>(); // node to return; function adds children (statements)

    unsigned i = 0;
    while (i < vec.size()) {
        if (vec.at(i).data == "if" || vec.at(i).data == "while") {
            unique_ptr<AST3::Node> nodeChild = make_unique<AST3::Node>();
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "condition";

            // getting condition
            i++; 
            vector<token> condition;
            while(vec.at(i).data != "{") {
                condition.push_back(vec.at(i));
                i++;
            }
            token emptyToken;
            unique_ptr<AST2::Node> conditionTree = build(condition, emptyToken);
            nodeChild->children.push_back(ConvertAST2ToAST3(conditionTree));
            // got condition, pushed as first index of nodeChild

            i++;
            // index at first token within block

            vector<token> blockVec = parseBlock(i, vec);
            // index at }

            for (unsigned j = 0; j < buildProgram(blockVec)->children.size(); j++) {
                nodeChild->children.push_back(move(buildProgram(blockVec)->children.at(j)));
            }
            node->children.push_back(move(nodeChild));
        }
        else if (vec.at(i).data == "else") {
            unique_ptr<AST3::Node> nodeChild = make_unique<AST3::Node>();
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "condition";

            if (vec.at(i + 1).data == "if") { // if "else if"
                i++;

                // create nodeGrandChild for nested "if" in case of "else if"
                unique_ptr<AST3::Node> nodeGrandChild = make_unique<AST3::Node>();
                nodeGrandChild->data = vec.at(i).data;
                nodeGrandChild->type = "condition";

                // getting condition
                i++;
                vector<token> condition;
                while(vec.at(i).data != "{") {
                    condition.push_back(vec.at(i));
                    i++;
                }
                token emptyToken;
                unique_ptr<AST2::Node> conditionTree = build(condition, emptyToken);
                nodeGrandChild->children.push_back(ConvertAST2ToAST3(conditionTree));
                // got condition, pushed as first index of nodeGrandChild
                
                i++;    
                // index at first token within block

                vector<token> blockVec = parseBlock(i, vec);
                // index at }

                unique_ptr<AST3::Node> block(new AST3::Node);
                block = buildProgram(blockVec);
                for (unsigned j = 0; j < block->children.size(); j++) {
                    nodeGrandChild->children.push_back(move(block->children.at(j)));
                }

                nodeChild->children.push_back(move(nodeGrandChild));
                
                if (i < vec.size() - 2) { // checking if "else" following "else if"
                    i++;
                    if (vec.at(i).data == "else") {
                        unique_ptr<AST3::Node> nodeElseChild = make_unique<AST3::Node>();
                        nodeElseChild->data = vec.at(i).data;
                        nodeElseChild->type = "condition";

                        i++;
                        i++;
                        // index at first token within block

                        vector<token> blockVec = parseBlock(i, vec);
                        // index at }

                        for (unsigned j = 0; j < buildProgram(blockVec)->children.size(); j++) {
                            nodeElseChild->children.push_back(move(buildProgram(blockVec)->children.at(j)));
                        }
                        nodeChild->children.push_back(move(nodeElseChild));
                    }
                }
                
                node->children.push_back(move(nodeChild));
                continue;
            }
            else { // if regular else statement (not "else if")
                i++;
                i++;
                // index at first token within block

                vector<token> blockVec = parseBlock(i, vec);
                // index at }

                for (unsigned j = 0; j < buildProgram(blockVec)->children.size(); j++) {
                    nodeChild->children.push_back(move(buildProgram(blockVec)->children.at(j)));
                }
                node->children.push_back(move(nodeChild));
            }
        }            
        else if (vec.at(i).type == "var") {
            int row = vec.at(i).row; // set row to current row
            vector<token> express;
            while (vec.at(i).row == row) {
                express.push_back(vec.at(i));
                i++;
                if (i > vec.size() - 1) {
                    break;
                }
            }
            token emptyToken;
            unique_ptr<AST2::Node> treeExpress = build(express, emptyToken);
            node->children.push_back(ConvertAST2ToAST3(treeExpress));
        }
        else if (vec.at(i).type == "print") {
            unique_ptr<AST3::Node> printNode = make_unique<AST3::Node>();
            printNode->data = "print";
            printNode->type = "print";

            // getting the expression that is being printed
            i++;
            int row = vec.at(i).row;
            vector<token> output;
            while(i < vec.size() && vec.at(i).row == row) {
                output.push_back(vec.at(i));
                i++;
            }
            token emptyToken;
            unique_ptr<AST2::Node> outputTree = build(output, emptyToken);
            printNode->children.push_back(ConvertAST2ToAST3(outputTree));
            node->children.push_back(move(printNode)); 
        }
        else {
            i++;
        }
    }  
    return node;
}

bool enterStatement (unique_ptr<AST3::Node> &root, vector<variable> &variables) { // helper function for runProgram; return true if condition is true to enter block
    unique_ptr<AST2::Node> ast2 = ConvertAST3ToAST2(root->children.at(0));
    boolNum condition;

    condition = evaluate(ast2, variables);

    if (condition.mType == "bool") {
        if (root->data == "if") {
            if (condition.mBool == true) {
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
        condition = evaluate(ast2, variables);
  
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
        }
        else {
            error Error;
            Error.code = 5;
            throw(Error);
        }
    }

    bool entered = false; // bool to signal previous conditional entered for "if" and "else"
    for (; i < root->children.size(); i++) {
        string kidType = root->children.at(i)->type;
        string kidData = root->children.at(i)->data;

        if (kidType == "op" || kidType == "eq" || kidType == "eqIneq" || kidType == "logicOp") {
            entered = false; // reset entered

            // call evaluate function and save result into variables 
            unique_ptr<AST2::Node> ast2Root = ConvertAST3ToAST2(root->children.at(i));
            boolNum result;
            result = evaluate(ast2Root, variables);
        }
        else if (kidData == "print") {
            entered = false; // reset entered
            unique_ptr<AST2::Node> ast2Root = ConvertAST3ToAST2(root->children.at(i)->children.at(0));
            boolNum output;
            output = evaluate(ast2Root, variables);

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