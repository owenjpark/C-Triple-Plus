# CheckPoint 1 

# Lexer 

The lexer takes in an input and returns a vector. To do so the lexer uses these helper functions:

- createTokens(const string line, const int row, vector<token> &inputVec)
- printTokens(vector<token> someVec)
- void addEndToken(vector<token> &inputVec, bool wasNewLine, int newlineCounter) 

# Parser 

The parser takes in a vector of tokens and contructs an AST. The operators and numbers are saved into nodes with the operands becoming the operator's children. This tree can then be printed in infix form and evaluated. The parser uses an AST class which includes the node struct, a destructor, and constructor. The parser also uses the following helper functions:

- calls the lexer to get a token vector

- Checks if the vector is a valid equation using expressionChecker(tokenVec) which makes sure:
    - there are valid parenthesis
    - there are not multiple expressions
    - "(" are followed by an operator

- Constructs an AST with the class constructor

- Calls createAST(vector<token> tokenVec, int index) with the root of the constructor://can delete int index param?
    - counts the left and right parenthesis to make sure the expression is complete
    - recursively calls createAST when a token.data = "(" and adds it the the nodes vector of children
    - each operator has a vector a children containing numbers and operators contained within its parentheses

- printInfix(AST::node* nodeParam) prints the resulting AST in infix form
- double evaluateAST(AST::node* nodeParam) returns the result of the tree 
    - errors with exit code 3 if anything gets divided by 0. 


