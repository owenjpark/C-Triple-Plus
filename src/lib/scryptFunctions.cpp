#include "lex.h"
#include "scrypt.h"
#include "calc.h"

// helper function; converts AST2 to AST3; returns root node

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
bool elseIf (const vector<token> &vec, unsigned &i, shared_ptr<AST2::Node> &node) {
    // assume at index }
    if (i < vec.size() - 2) { // checking if "else" following "else if" 
        if (i <vec.size() - 3)  {
            if (vec.at(i + 1).data == "else" && (vec.at(i + 2).data == "if")) { // for consecutive "else ifs"
                i++; 
                // index at "else"
                shared_ptr<AST2::Node> nodeChild = make_shared<AST2::Node>();
                nodeChild->data = vec.at(i).data;
                nodeChild->type = "condition";

                i++;
                // index at "if"
                shared_ptr<AST2::Node> nodeGrandChild = make_shared<AST2::Node>();
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
                nodeGrandChild->children.push_back(conditionTree);

                i++;
                // index at first token within block

                vector<token> blockVec = parseBlock(i, vec);
                // index at }

                shared_ptr<AST2::Node> block(new AST2::Node);
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
            shared_ptr<AST2::Node> nodeElseChild = make_shared<AST2::Node>();
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

shared_ptr<AST2::Node> buildProgram(const vector<token> &vec) {
    shared_ptr<AST2::Node> node = make_shared<AST2::Node>(); // node to return; function adds children (statements)

    unsigned i = 0;
    while (i < vec.size()) {
        if (vec.at(i).data == "if" || vec.at(i).data == "while") {
            shared_ptr<AST2::Node> nodeChild = make_shared<AST2::Node>();
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
            nodeChild->children.push_back(conditionTree);
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
        else if (vec.at(i).data == "def") {
            //cout << "making definition" << endl;
            shared_ptr<AST2::Node> nodeChild = make_shared<AST2::Node>();
            i++;
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "funcDef";

            // going to first identifier "skipping ("
            i++;
            i++;
            vector<variable> identifiers;

            while (i < vec.size() && vec.at(i).data != ")") {
                variable param;
                param.type = "identity";
                // grabbing each identifier which is always a variable in defintion
                if (vec.at(i).data != ",") {
                    param.name = vec.at(i).data;
                    identifiers.push_back(param);
                }
                i++;
            }
            nodeChild->scope = identifiers; // pushing all identifiers into the var vector 

            i += 2;  // getting into {

            // building the body of the function
            vector<token> blockVec = parseBlock(i, vec);
            // index at }

            for (unsigned j = 0; j < buildProgram(blockVec)->children.size(); j++) {
                nodeChild->children.push_back(move(buildProgram(blockVec)->children.at(j)));
            }
            //cout << nodeChild->children[0]->children[0]->data << endl;
            node->children.push_back(move(nodeChild));

        }

        else if (vec.at(i).data == "else") {
            shared_ptr<AST2::Node> nodeChild = make_shared<AST2::Node>();
            nodeChild->data = vec.at(i).data;
            nodeChild->type = "condition";

            if (vec.at(i + 1).data == "if") { // if "else if"
                i++;

                // create nodeGrandChild for nested "if" in case of "else if"
                shared_ptr<AST2::Node> nodeGrandChild = make_shared<AST2::Node>();

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
                nodeGrandChild->children.push_back(conditionTree);
                // got condition, pushed as first index of nodeGrandChild
                
                i++;    
                // index at first token within block

                vector<token> blockVec = parseBlock(i, vec);
                // index at }

                shared_ptr<AST2::Node> block(new AST2::Node);
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
        else if (vec.at(i).type == "var" || vec.at(i).type == "name") {
            vector<token> express;
            while (vec.at(i).data != ";") {
                express.push_back(vec.at(i));
                i++;
                if (i > vec.size() - 1) {
                    break;
                }
            }
            // index at semi-colon
            i++;
            shared_ptr<AST2::Node> treeExpress = build(express);
            node->children.push_back(treeExpress);
        }
        else if (vec.at(i).type == "print") {
            shared_ptr<AST2::Node> printNode = make_shared<AST2::Node>();
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
            printNode->children.push_back(outputTree);
            node->children.push_back(move(printNode)); 
        }
        
        else {
            i++;
        }
    }  
    return node;
}

// helper function for runProgram; return true if condition is true to enter block
bool enterStatement (const shared_ptr<AST2::Node> &root, vector<variable> &variables) {
    shared_ptr<AST2::Node> ast2 = root->children.at(0);
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

void runProgram(const shared_ptr<AST2::Node> &root, vector<variable> &variables) {
    //cout << 1 << endl;
    unsigned i = 0;
    if (root->data == "if" || root->data == "else if" || root->data == "while") {
        shared_ptr<AST2::Node> ast2 = root->children.at(0);
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
    //cout << root->children[0]->children[0]->data << endl;
    for (; i < root->children.size(); i++) {
        string kidType = root->children.at(i)->type;
        //cout << "kidType" << endl;
        string kidData = root->children.at(i)->data;

        if (kidType == "funcDef") {
            //cout << "running in funcDef" << endl;
            // add funcDef to variable list 
            variable var("funcDef", kidData);
            //cout << kidData << endl;
            var.definition = root->children.at(i);
            variables.push_back(var);

            //TO DO:
                // go through body and see if any variables need to be assigned a value 
                // update funcDef variable vector if yes
            // f = foo()
            // x = f(1,2)
        }
        else if (kidType == "funCall") {
            //cout << "in funCall" << endl;
            // find func name in scope
            // iterate data string
            int j = 0;
            string info = root->children.at(i)->data;


            string name;
            while (info.at(j) != '(') {
                name += info.at(j);
                j++;
            }


            j++; // going into params and building all the parmeters for local scope
            vector<shared_ptr<AST2::Node>> paramExpress;
            while (info.at(j) != ')') {
                //cout << "ajdheoiufwhi" << endl;
                string expression;

                //getting each parameter
                //cout << "b4 2nd while" << endl;
                while (info.at(j) != ',' && info.at(j) != ')') {
                    //cout << "after" << endl;
                    expression += info.at(j);
                    j++;
                }
                
                    //making each parameter into a tree
                    vector<token> express;
                    createTokens(expression,1, express);
                    shared_ptr<AST2::Node> expressNode = build(express);


                    paramExpress.push_back(expressNode);
                if (info.at(j) != ')') {
                    j++;
                }
            }


            // find defintion in global scope
            shared_ptr<AST2::Node> funcBody;
            for (int i = 0; i < int(variables.size()); i++) { // check if variable exists, update it
                if (variables[i].name == name) {
                    //cout << "found function in variable!" << endl;
                    funcBody = variables[i].definition;
                }
            }


            //assign identifiers to input parameters
            vector<variable> empty;
            unsigned int localParam = paramExpress.size();
            //cout << "amount: "<< localParam << endl;
            if (localParam < funcBody->scope.size()) {
                error argCount;
                argCount.code = 6;
                throw(argCount);
            }
            for (unsigned int m = 0; m < localParam; m++){
                boolNum result = evaluate(paramExpress[m], empty);
                //cout << result.mNum <<endl;
                if (result.mType == "bool") {
                funcBody->scope[m].boolValue = result.mBool;
                funcBody->scope[m].type = "bool";
                }
                if (result.mType == "num") {
                funcBody->scope[m].numValue = result.mNum;
                funcBody->scope[m].type = "num";
                }
            }

            //run function body with new variable scope vector
            //cout << funcBody->children.size();
            //cout << funcBody->children[0]->data;
            runProgram(funcBody, funcBody->scope);
            /* for (unsigned int j=0; j < funcBody->children.size(); j++ ) {
                cout << funcBody->children[j]->data << endl;
                runProgram(funcBody->children[j], funcBody->scope);
            } */

        }
        else if (kidType == "op" || kidType == "eq" || kidType == "eqIneq" || kidType == "logicOp") {
            entered = false; // reset entered
            // check if it contains a funCall
            // call evaluate function and save result into variables 
            shared_ptr<AST2::Node> ast2Root = root->children.at(i);
            boolNum result;
            result = evaluate(ast2Root, variables);
        }
        else if (kidData == "print") {
            entered = false; // reset entered
            shared_ptr<AST2::Node> ast2Root = root->children.at(i)->children.at(0);
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
        else if (kidData == "return") {
            entered = false; // reset entered
            shared_ptr<AST2::Node> ast2Root = root->children.at(i)->children.at(0);
            boolNum output;
            output = evaluate(ast2Root, variables);
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


    }
    if (root->data == "while") { // continue running until while condition false
        runProgram(root, variables);
    }
}