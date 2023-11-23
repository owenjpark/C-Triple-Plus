#include "lex.h"
#include "scrypt.h"
#include "calc.h"

// helper function; converts AST2 to AST3; returns root node
shared_ptr<AST3::Node> ConvertAST2ToAST3(const shared_ptr<AST2::Node> &node2) { 
    shared_ptr<AST3::Node> node3 = make_shared<AST3::Node>();

    node3->type = node2->type;
    node3->data = node2->data;

    vector<shared_ptr<AST3::Node>> array3; // convert the array<AST2::Node> to array <AST3::Node> in array3
    for (unsigned i = 0; i < node2->array.size(); i++) {
        array3.push_back(ConvertAST2ToAST3(node2->array.at(i)));
    }
    node3->array = array3;
   

    if (node2->leftChild) {
        node3->children.push_back(ConvertAST2ToAST3(node2->leftChild));
    }
    if (node2->rightChild) {
        node3->children.push_back(ConvertAST2ToAST3(node2->rightChild));
    }
    return node3;
}

// helper function; converts AST3 to AST2; returns root node
shared_ptr<AST2::Node> ConvertAST3ToAST2(const shared_ptr<AST3::Node> &node3) { 
    shared_ptr<AST2::Node> node2 = make_shared<AST2::Node>();

    node2->type = node3->type;
    node2->data = node3->data;

    vector<shared_ptr<AST2::Node>> array2; // convert the array<AST2::Node> to array <AST3::Node> in array2
    for (unsigned i = 0; i < node3->array.size(); i++) {
        array2.push_back(ConvertAST3ToAST2(node3->array.at(i)));
    }
    node2->array = array2;
    
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


// helper function for build program; creates a vector of all tokens within block
vector<token> parseBlock(unsigned &i, const vector<token> &vec) { 
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

// helper function for bulid program; deals with consecutive "else ifs" and "else" after "else if"
bool elseIf (const vector<token> &vec, unsigned &i, shared_ptr<AST3::Node> &node) {
    // assume at index }
    if (i < vec.size() - 2) { // checking if "else" following "else if" 
        if (i <vec.size() - 3)  {
            if (vec.at(i + 1).data == "else" && (vec.at(i + 2).data == "if")) { // for consecutive "else ifs"
                i++; 
                // index at "else"
                shared_ptr<AST3::Node> nodeChild = make_shared<AST3::Node>();
                nodeChild->data = vec.at(i).data;
                nodeChild->type = "condition";

                i++;
                // index at "if"
                shared_ptr<AST3::Node> nodeGrandChild = make_shared<AST3::Node>();
                nodeGrandChild->data = vec.at(i).data;
                nodeGrandChild->type = "condition";

                i++;
                // index at condition
                vector<token> condition;
                while(vec.at(i).data != "{") {
                    condition.push_back(vec.at(i));
                    i++;
                }
                shared_ptr<AST2::Node> conditionTree = build(condition);
                nodeGrandChild->children.push_back(ConvertAST2ToAST3(conditionTree));

                i++;
                // index at first token within block

                vector<token> blockVec = parseBlock(i, vec);
                // index at }

                shared_ptr<AST3::Node> block(new AST3::Node);
                block = buildProgram(blockVec);
                for (unsigned j = 0; j < block->children.size(); j++) {
                    nodeGrandChild->children.push_back(move(block->children.at(j)));
                }

                nodeChild->children.push_back(move(nodeGrandChild));
                elseIf(vec, i, nodeChild);
                node->children.push_back(move(nodeChild));
                return false;
            }
        }
        if (vec.at(i + 1).data == "else") { // for "else" after "else if"
            i++;
            shared_ptr<AST3::Node> nodeElseChild = make_shared<AST3::Node>();
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
            node->children.push_back(move(nodeElseChild));
        }
    }
    return false;
}

shared_ptr<AST3::Node> buildProgram(const vector<token> &vec) {
    shared_ptr<AST3::Node> node = make_shared<AST3::Node>(); // node to return; function adds children (statements)

    unsigned i = 0;
    while (i < vec.size()) {
        if (vec.at(i).data == "if" || vec.at(i).data == "while") {
            shared_ptr<AST3::Node> nodeChild = make_shared<AST3::Node>();
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "condition";

            // getting condition
            i++; 
            vector<token> condition;
            while(vec.at(i).data != "{") {
                condition.push_back(vec.at(i));
                i++;
            }
            shared_ptr<AST2::Node> conditionTree = build(condition);
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
            shared_ptr<AST3::Node> nodeChild = make_shared<AST3::Node>();
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "condition";

            if (vec.at(i + 1).data == "if") { // if "else if"
                i++;

                // create nodeGrandChild for nested "if" in case of "else if"
                shared_ptr<AST3::Node> nodeGrandChild = make_shared<AST3::Node>();

                nodeGrandChild->data = vec.at(i).data;
                nodeGrandChild->type = "condition";

                // getting condition
                i++;
                vector<token> condition;
                while(vec.at(i).data != "{") {
                    condition.push_back(vec.at(i));
                    i++;
                }
                shared_ptr<AST2::Node> conditionTree = build(condition);
                nodeGrandChild->children.push_back(ConvertAST2ToAST3(conditionTree));
                // got condition, pushed as first index of nodeGrandChild
                
                i++;    
                // index at first token within block

                vector<token> blockVec = parseBlock(i, vec);
                // index at }

                shared_ptr<AST3::Node> block(new AST3::Node);
                block = buildProgram(blockVec);
                for (unsigned j = 0; j < block->children.size(); j++) {
                    nodeGrandChild->children.push_back(move(block->children.at(j)));
                }

                nodeChild->children.push_back(move(nodeGrandChild));

                while (elseIf(vec, i, nodeChild)); // deals with possible consecutive "if elses" and "else" after "else if"
                
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
            vector<token> express;
            while (vec.at(i).data != ";") {
                express.push_back(vec.at(i));
                i++;
            }
            // index at semi-colon
            i++;
            shared_ptr<AST2::Node> treeExpress = build(express);
            node->children.push_back(ConvertAST2ToAST3(treeExpress));
        }
        else if (vec.at(i).type == "name") { // for funcCall
            vector<token> express;
            while (vec.at(i).data != ";") {
                express.push_back(vec.at(i));
                i++;
            }
            // index at semi-colon
            i++;
            shared_ptr<AST2::Node> treeExpress = build(express);
            node->children.push_back(ConvertAST2ToAST3(treeExpress));
        }
        else if (vec.at(i).type == "print") {
            shared_ptr<AST3::Node> printNode = make_shared<AST3::Node>();
            printNode->data = "print";
            printNode->type = "print";

            // getting the expression that is being printed
            i++;
            vector<token> output;
            while(i < vec.size() && vec.at(i).data != ";") {
                output.push_back(vec.at(i));
                i++;
            }
            // index at semi-colon
            i++;
            shared_ptr<AST2::Node> outputTree = build(output);
            printNode->children.push_back(ConvertAST2ToAST3(outputTree));
            node->children.push_back(move(printNode)); 
        }
        else if (vec.at(i).type == "return") {
            shared_ptr<AST3::Node> returnNode = make_shared<AST3::Node>();
            returnNode->type = "return";
            returnNode->data = "return";
            
            // getting the expression that is being returned
            i++;
            vector<token> returnVal;
            while(i < vec.size() && vec.at(i).data != ";") {
                returnVal.push_back(vec.at(i));
                i++;
            }
            // index at semi-colon
            i++;
            if (returnVal.size() != 0) {
                shared_ptr<AST2::Node> returnValTree = build(returnVal);
                returnNode->children.push_back(ConvertAST2ToAST3(returnValTree));
                node->children.push_back(move(returnNode)); 
            }
        }
        else if (vec.at(i).data == "def") {
            i++;
            // at func name

            shared_ptr<AST3::Node> nodeChild = make_shared<AST3::Node>(); // nodeChild = func node
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "def";

            // going to first parameter "skipping ("
            i++;
            i++;
            // at first param

            vector<shared_ptr<AST3::Node>> grandChildren; // grandChildren are the params and * statements
            while (i < vec.size() && vec.at(i).data != ")") {
                // grabbing each parameter
                if (vec.at(i).data != ",") { 
                    shared_ptr<AST3::Node> grandChild = make_shared<AST3::Node>();
                    grandChild->type = "parameter";
                    grandChild->data = vec.at(i).data;
                    grandChildren.push_back(grandChild);
                }
                i++;
            }
            nodeChild->children = grandChildren; // pushing all params to def children

            i++;  
            i++;
            // i at the block
            
            // building the body of the function
            vector<token> blockVec = parseBlock(i, vec);
            // index at }

            if (blockVec.size() != 0) { // to prevent from building with empty vec
                nodeChild->children.push_back(buildProgram(blockVec));
            }
            node->children.push_back(move(nodeChild));
        }
        else {
            i++;
        }
    }  
    return node;
}

// helper function for runProgram; return true if condition is true to enter block
bool enterStatement (const shared_ptr<AST3::Node> &root, vector<variable> &variables) {
    shared_ptr<AST2::Node> ast2 = ConvertAST3ToAST2(root->children.at(0));
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

Value runProgram(const shared_ptr<AST3::Node> &root, vector<variable> &variables) {
    unsigned i = 0;
    if (root->data == "if" || root->data == "else if" || root->data == "while") {
        shared_ptr<AST2::Node> ast2 = ConvertAST3ToAST2(root->children.at(0));
        boolNum condition;
        condition = evaluate(ast2, variables);
  
        if (condition.mType == "bool") {
            if (root->data == "if") {
                if (condition.mBool == true) { // continue to run program
                    i++;
                }
                else { // stop running program
                    return "null";
                }
            }
            else if (root->data == "while") {
                if (condition.mBool == true) { // continue to run program
                    i++;
                }
                else { // stop running program
                    return "null";
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
            shared_ptr<AST2::Node> ast2Root = ConvertAST3ToAST2(root->children.at(i));
            boolNum result;

            result = evaluate(ast2Root, variables);
        }
        else if (kidData == "print") {
            entered = false; // reset entered
            shared_ptr<AST2::Node> ast2Root = ConvertAST3ToAST2(root->children.at(i)->children.at(0)); // evaluate child of print
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
            else if(output.mType == "null") {
                cout << "null" << endl;
            }
            else if (output.mType == "array") {
                arrayPrinter(output.mArray);
                cout << endl;
            }
        }
        else if (kidData == "return") {
            entered = false; // reset entered
            if (root->children.at(i)->children.size() != 0) {
                shared_ptr<AST2::Node> ast2Root = ConvertAST3ToAST2(root->children.at(i)->children.at(0));
                boolNum output;
                output = evaluate(ast2Root, variables);

                if (output.mType == "num") {
                    return output.mNum; 
                }
                else if (output.mType == "bool") {
                    return output.mBool;
                }
                else if (output.mType == "array") {
                    return output.mArray;
                }
                else if (output.mType == "null") {
                    return "null";
                }
            }
            else {
                return "null"; // for return;
            }
        }
        // for nested conditionals
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
        else if (kidType == "def") { // for function definition
            variable funcVar; // will be pushed onto variables
            funcVar.type = "func";
            funcVar.name = kidData; // name of functions
            functionVal funcVal; // will be put into funcVar
            funcVal.localScope = vector<variable>(variables); // copy constructor to copy global Variables
            for (unsigned j = 0; j < root->children.at(i)->children.size(); j++) { // run through children of func node
                if (root->children.at(i)->children.at(j)->type == "parameter") { // if parameters
                    variable parameter;
                    parameter.type = "parameter";
                    parameter.name = root->children.at(i)->children.at(j)->data;
                    funcVal.localScope.push_back(parameter);
                }
                else { // else statements
                    funcVal.statements = root->children.at(i)->children.at(j);  
                }
            }
            funcVar.funcVal = funcVal;
            variables.push_back(funcVar);
        }
        else if (kidType == "funcCall") {
            shared_ptr<AST2::Node> convertedNode = ConvertAST3ToAST2(root->children.at(i));
            for (unsigned i = 0; i < variables.size(); i++) {
            }
            evaluate(convertedNode, variables);
        }
    }
    if (root->data == "while") { // continue running until while condition false
        runProgram(root, variables);
    }
    return "null";
}

void validReturn(shared_ptr<AST3::Node> root, bool inFunc) { // helper function to check if all returns are within functions
    if (root->type == "return" && !inFunc) {
        error unexpectedReturn;
        unexpectedReturn.code = 11;
        throw(unexpectedReturn);
    }
    if (root->type == "def") {
        inFunc = true;
    }
    for (unsigned i = 0; i < root->children.size(); i++) {
        if (root->children.at(i)->type == "name") {
            for (unsigned j = 0; j < root->children.at(i)->array.size(); j++) {
                validReturn(root->children.at(i)->array.at(j), inFunc);
            }
        }
        validReturn(root->children.at(i), inFunc);
    }
}