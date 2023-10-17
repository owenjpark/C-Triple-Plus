# S-Expression Calculator

This project is split into two parts:
1. Lexer - tokenizes and input
2. Parser - goes through tokens, converts to infix form, and evaluates expression
   
# Lexer 
Split into 3 files:
1. lex.h - header file of all fucntion and class declarations
2. lexfunctions.cpp - implementation of functions
3. lex.cpp - takes in input, uses functions, and produces output

The lexer takes in an input and prints out all of the tokens (parenthesis, operator, number) with their corresponding row and column # in the input. To do so the lexer uses these helper functions:
- createTokens() - takes in a line of input and creates tokens and adds them to token vector
- addEndToken() - when input is finished, adds endtoken
- lexer() - takes input and uses previous two functions to return a completed vector of tokens
- printToken() - prints tokens

To build the lexer run the following command:
```
g++ -Wall -Wextra -Werror -std=c++17 lib/lexfunctions.cpp lex.cpp
```

# Parser 
Split into 3 files: 
1. parse.h - header file of all fucntion and class declarations
2. parsefunctions.cpp - implementation of functions
3. parse.cpp - takes in input, uses functions, and produces output
   
The parser takes in a vector of tokens and contructs an AST. The operators and numbers are saved into nodes with the operands becoming the operator's children. This tree can then be printed in infix form and evaluated. The parser uses an AST class which includes the node struct, a destructor, and constructor. The parser also uses the following helper functions:

- lexer() gives vector of tokens to parse
- expressionChecker() checks that valid expression can be created from the tokens
- createAST() creates and AST from tokens
- parser() combines the previous two functions running expressionChecker() first then createAST()
- printInfix() prints out AST in infix form
- evaulate() evaluates expression represented by the AST

To build the lexer run the following command:
```
g++ -Wall -Wextra -Werror -std=c++17 lib/lexfunctions.cpp lib/parsefunctions.cpp parse.cpp
```


