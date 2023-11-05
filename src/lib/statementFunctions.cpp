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
           // cout << "in condition!!" << endl;
            if (vec[i].data == "if" || vec[i].data == "while" || vec[i].data == "else if") {
                nodeChild->data = vec[i].data; 
                token parent = vec[i];
           // cout << "if or while or else if " << endl;

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
            }
            // getting what is inside of {}
            vector<token> actions;
            while(vec[i].data != "}") {
                actions.push_back(vec[i]);
                i++;
            }
            
            node->children.push_back(buildProgram(actions));
        }

        // expressions 
        else if (vec[i].type == "var") {
            // grab the entire expression
            //cout << "in var!!!" << endl;
            
            int row = vec[i].row;
            vector<token> express;
            while (vec[i].row == row) {
               // cout << "in var while loop" << endl;
                express.push_back(vec[i]);
                i++; /// gets the while 
                if (i > int(vec.size()) -1 ) break;
                //cout << "past break" << endl;
                if (vec[i].type == "end") break;  
            }
            token parent = vec[i];
            //cout << "made it here" << endl;
            std::unique_ptr<AST2::Node> treeExpress;

            try { // build tree
                    treeExpress = build(express, parent);
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
            node->data = "print";
            node->type = "print";
            i++;
            std::unique_ptr<AST3::Node> printNode = std::make_unique<AST3::Node>();
            printNode->data = vec[i].data;
            printNode->type = vec[i].type;
            node->children.push_back(std::move(printNode)); 
            i++;
        }
    
        else i++;
        
    } 
    
    return node;
}; 



