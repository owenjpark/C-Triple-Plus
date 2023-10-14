#include "lib/parse.h"
#include "lib/lex.h"
#include <vector>
#include <iomanip>

//constructing the tree
//check if parenthesis match if not error  Unexpected token at line L column C: TOKEN  exit code 2 
// go unti
//AST
AST::AST(){
    root = nullptr;
}

//destructor 
AST::~AST(){
}

//converting to a string of the infix form 
string AST::equation(){ }

// calculating the tree
float AST::answer(){
    // if number 
    // if operator 
    // if ( )

    //Runtime error: division by zero.  exit code three
    
}
AST::token* parse(vector<vecComponent> formula, int i=0){
    
    string data = formula[i].data;

    // base case 
    if (data == ")") {
        
        // base case activity return the token*
    }

    else if (data == "(") {
        //get operator 
        i += 1;
        AST::token op;
        data = formula[i].data;
        op.data = data;

        i += 1;
        while (data != "(" || data != ")") {
            AST::token* num = new AST::token();
            num->data = formula[i].data;
            i += 1;
            
            op.children.push_back(num);

        }

        if (data == "(") {
        op.children.push_back(parse(formula, i));
        }
    }

    else if (data == "/" || data == "*" || data == "+" || data == "-") {
        AST::token op;
        op.data = data;
        
    }

}


// parse function takes declares an AST        // excepts vector
// reads through the token list 
// adds the tokens to the tree
// returns the tree
